#pragma once
#include "../utils/NeigeDefines.h"
#include <stdexcept>
#include <bitset>
#include <queue>
#include <array>
#include <unordered_map>
#include <memory>
#include <set>

#define MAX_ENTITIES 4096
#define MAX_COMPONENTS 32

using Entity = uint32_t;
using Component = uint8_t;
using ComponentMask = std::bitset<MAX_COMPONENTS>;

class EntityManager {
public:
	EntityManager() {
		for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
			entities.push(entity);
		}
	}

	Entity createEntity() {
		NEIGE_ASSERT(numberOfEntities < MAX_ENTITIES, "Too much entities (" + std::to_string(numberOfEntities) + " entities, MAX = " + std::to_string(MAX_ENTITIES) + ").");

		Entity id = entities.front();
		entities.pop();
		numberOfEntities++;

		return id;
	}

	void destroyEntity(Entity entity) {
		NEIGE_ASSERT(entity < numberOfEntities, "Entity " + std::to_string(entity) + " is not in range (destroy entity).");

		componentMasks[entity].reset();
		entities.push(entity);
		numberOfEntities--;
	}

	void setComponents(Entity entity, ComponentMask componentMask) {
		NEIGE_ASSERT(entity < numberOfEntities, "Entity " + std::to_string(entity) + " is not in range (set components).");

		componentMasks[entity] = componentMask;
	}

	ComponentMask getComponents(Entity entity) {
		NEIGE_ASSERT(entity < numberOfEntities, "Entity " + std::to_string(entity) + " is not in range (get components).");

		return componentMasks[entity];
	}
private:
	std::queue<Entity> entities;
	std::array<ComponentMask, MAX_ENTITIES> componentMasks;
	uint32_t numberOfEntities = 0;
};

class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
	void insertData(Entity entity, T component) {
		NEIGE_ASSERT(entityToIndex.find(entity) == entityToIndex.end(), "Component added to same entity (insert data).");

		size_t tmp = validSize;
		entityToIndex[entity] = tmp;
		indexToEntity[tmp] = entity;
		components[tmp] = component;
		validSize++;
	}

	void removeData(Entity entity) {
		NEIGE_ASSERT(entityToIndex.find(entity) != entityToIndex.end(), "Component \"" + std::to_string(entity) + "\" does not exist (remove data).");

		size_t tmp = entityToIndex[entity];
		components[tmp] = components[validSize - 1];
		Entity entityLast = indexToEntity[validSize - 1];
		entityToIndex[entityLast] = tmp;
		indexToEntity[tmp] = entityLast;
		entityToIndex.erase(entity);
		indexToEntity.erase(validSize - 1);
		validSize--;
	}

	T& getData(Entity entity) {
		NEIGE_ASSERT(entityToIndex.find(entity) != entityToIndex.end(), "Component \"" + std::to_string(entity) + "\" does not exist (get data).");

		return components[entityToIndex[entity]];
	}

	void entityDestroyed(Entity entity) override {
		if (entityToIndex.find(entity) == entityToIndex.end()) {
			removeData(entity);
		}
	}
private:
	std::array<T, MAX_ENTITIES> components;
	std::unordered_map<Entity, size_t> entityToIndex;
	std::unordered_map<size_t, Entity> indexToEntity;
	size_t validSize;
};

class ComponentManager {
public:
	template<typename T>
	void registerComponent() {
		const char* typeName = typeid(T).name();
		
		NEIGE_ASSERT(componentTypes.find(typeName) == componentTypes.end(), "Component \"" + std::string(typeName) + "\" is already registered (register component).");

		componentTypes.insert({ typeName, nextComponent });
		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });
		nextComponent++;
	}

	template<typename T>
	Component getComponentId() {
		const char* typeName = typeid(T).name();

		NEIGE_ASSERT(componentTypes.find(typeName) != componentTypes.end(), "Component \"" + std::string(typeName) + "\" does not exist (get component).");

		return componentTypes[typeName];
	}

	template<typename T>
	void addComponent(Entity entity, T component) {
		getComponentArray<T>()->insertData(entity, component);
	}

	template<typename T>
	void removeComponent(Entity entity) {
		getComponentArray<T>()->removeData(entity);
	}

	template<typename T>
	T& getComponent(Entity entity) {
		return getComponentArray<T>()->getData(entity);
	}

	void entityDestroyed(Entity entity) {
		for (auto const& pair : componentArrays) {
			auto const& componentArray = pair.second;
			componentArray->entityDestroyed(entity);
		}
	}
private:
	std::unordered_map<const char*, Component> componentTypes;
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays;
	Component nextComponent = 0;

	template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray() {
		const char* typeName = typeid(T).name();

		NEIGE_ASSERT(componentTypes.find(typeName) != componentTypes.end(), "Component \"" + std::string(typeName) + "\" does not exist (get component array).");

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}
};

class System {
public:
	std::set<Entity> entities;
};

class SystemManager {
public:
	template<typename T>
	std::shared_ptr<T> registerSystem() {
		const char* typeName = typeid(T).name();

		NEIGE_ASSERT(systems.find(typeName) == systems.end(), "System \"" + std::string(typeName) + "\" is already registered (register system).");

		std::shared_ptr<T> system = std::make_shared<T>();
		systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void setComponents(ComponentMask componentMask) {
		const char* typeName = typeid(T).name();

		NEIGE_ASSERT(systems.find(typeName) != systems.end(), "System \"" + std::string(typeName) + "\" does not exist (set components).");

		componentMasks.insert({ typeName, componentMask });
	}

	void entityDestroyed(Entity entity) {
		for (auto const& pair : systems) {
			auto const& system = pair.second;
			system->entities.erase(entity);
		}
	}

	void entityComponentMaskChanged(Entity entity, ComponentMask entityComponentMask) {
		for (auto const& pair : systems) {
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemComponentMask = componentMasks[type];
			if ((entityComponentMask & systemComponentMask) == systemComponentMask) {
				system->entities.insert(entity);
			} else {
				system->entities.erase(entity);
			}
		}
	}
private:
	std::unordered_map<const char*, ComponentMask> componentMasks;
	std::unordered_map<const char*, std::shared_ptr<System>> systems;
};

class ECS {
public:
	void init() {
		entityManager = std::make_unique<EntityManager>();
		componentManager = std::make_unique<ComponentManager>();
		systemManager = std::make_unique<SystemManager>();
	}

	// Entity
	Entity createEntity() {
		return entityManager->createEntity();
	}

	void destroyEntity(Entity entity) {
		entityManager->destroyEntity(entity);
		componentManager->entityDestroyed(entity);
		systemManager->entityDestroyed(entity);
	}

	// Component
	template<typename T>
	void registerComponent() {
		return componentManager->registerComponent<T>();
	}

	template<typename T>
	void addComponent(Entity entity, T component) {
		componentManager->addComponent<T>(entity, component);
		auto components = entityManager->getComponents(entity);
		components.set(componentManager->getComponentId<T>(), true);
		entityManager->setComponents(entity, components);
		systemManager->entityComponentMaskChanged(entity, components);
	}

	template<typename T>
	void removeComponent(Entity entity) {
		componentManager->removeComponent<T>(entity);
		auto components = entityManager->getComponents(entity);
		components.set(componentManager->getComponentId<T>(), false);
		entityManager->setComponents(entity, components);
		systemManager->entityComponentMaskChanged(components);
	}

	template<typename T>
	T& getComponent(Entity entity) {
		return componentManager->getComponent<T>(entity);
	}

	template<typename T>
	Component getComponentId() {
		return componentManager->getComponentId<T>();
	}

	// System
	template<typename T>
	std::shared_ptr<T> registerSystem() {
		return systemManager->registerSystem<T>();
	}

	template<typename T>
	void setSystemComponents(ComponentMask componentMask) {
		systemManager->setComponents<T>(componentMask);
	}
private:
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<SystemManager> systemManager;
};
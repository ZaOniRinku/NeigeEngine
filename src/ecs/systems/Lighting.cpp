#include "Lighting.h"
#include "../../graphics/resources/ShaderResources.h"

extern ECS ecs;

void Lighting::init() {
	for (Entity entity : entities) {
		lights.emplace(entity);
	}
	entities.clear();
}

void Lighting::update() {
	for (Entity entity : entities) {
		lights.emplace(entity);
	}
}
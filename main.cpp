#include "src/Game.h"
#include "src/ecs/ECS.h"
#include "src/ecs/components/Transform.h"
#include "src/ecs/components/Camera.h"
#include "src/ecs/components/Renderable.h"
#include "src/ecs/components/Light.h"
#include "src/utils/resources/ModelLoader.h"

ECS ecs;

int main(void) {
	ecs.init();

	Game g;

	Window w;
	w.extent.width = 480;
	w.extent.height = 360;

	g.window = &w;
	g.init();

	Entity entity1 = ecs.createEntity();
	ecs.addComponent(entity1, Renderable{
		"../modelfiles/Fox.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(entity1, Transform{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.01f, 0.01f, 0.01f)
		});
	
	Entity entity2 = ecs.createEntity();
	ecs.addComponent(entity2, Renderable{
		"../modelfiles/Duck2.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(entity2, Transform{
		glm::vec3(5.0f, 1.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.01f, 0.01f, 0.01f)
		});

	Entity entity3 = ecs.createEntity();
	ecs.addComponent(entity3, Renderable{
		"../modelfiles/DamagedHelmet.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(entity3, Transform{
		glm::vec3(4.0f, 1.0f, -2.0f),
		glm::vec3(90.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
		});

	Entity entity4 = ecs.createEntity();
	ecs.addComponent(entity4, Renderable{
		"../modelfiles/sphere.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(entity4, Transform{
		glm::vec3(5.0f, 1.0f, -4.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
		});

	Entity entity5 = ecs.createEntity();
	ecs.addComponent(entity5, Renderable{
		"../modelfiles/plane.glb",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(entity5, Transform{
		glm::vec3(5.0f, -1.0f, -4.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(10.0f, 1.0f, 10.0f)
		});

	Entity light1 = ecs.createEntity();
	ecs.addComponent(light1, Light{
		LightType::DIRECTIONAL,
		glm::vec3(0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec2(0.0f)
		});

	Entity light2 = ecs.createEntity();
	ecs.addComponent(light2, Light{
		LightType::SPOT,
		glm::vec3(5.0f, 2.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec2(40.0f, 50.0f)
		});

	Entity light3 = ecs.createEntity();
	ecs.addComponent(light3, Light{
		LightType::POINT,
		glm::vec3(2.0f, 0.0f, 2.0f),
		glm::vec3(0.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec2(0.0f)
		});

	g.launch();
}
#include "src/Game.h"
#include "src/ecs/ECS.h"
#include "src/ecs/components/Transform.h"
#include "src/ecs/components/Camera.h"
#include "src/ecs/components/Renderable.h"
#include "src/ecs/components/Light.h"
#include "src/ecs/components/Rigidbody.h"
#include <random>

ECS ecs;

int main(void) {
	ecs.init();

	Game g;

	Window w;
	w.extent.width = 720;
	w.extent.height = 480;

	g.window = &w;
	g.init();

	Entity sceneCamera = ecs.createEntity();
	ecs.addComponent(sceneCamera, Camera{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		45.0f,
		0.3f,
		1000.0f,
		"../modelfiles/sunset_forest_8k.hdr"
		});

	/*std::default_random_engine generator;
	std::uniform_real_distribution<float> randomPosition(-10.0f, 10.0f);

	std::vector<Entity> createdEntities(2000);
	for (float i = -1000.0f; i < 1000.0f; i++) {
			Entity e = ecs.createEntity();
			ecs.addComponent(e, Renderable{
			"../modelfiles/Sphere.gltf",
			"../shaders/pbr.vert",
			"../shaders/pbr.frag",
			"",
			"",
			"",
			Topology::TRIANGLE_LIST
				});
			ecs.addComponent(e, Transform{
				glm::vec3(randomPosition(generator), -2.0f, randomPosition(generator)),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(1.0f)
				});
	}*/

	Entity skinningTest = ecs.createEntity();
	ecs.addComponent(skinningTest, Renderable{
		"../modelfiles/BoomBox.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(skinningTest, Transform{
		glm::vec3(3.0f, -1.0f, 0.0f),
		glm::vec3(180.0f, 0.0f, 0.0f),
		glm::vec3(100.0f, 100.0f, 100.0f)
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
		glm::vec3(1.0f, 1.0f, 1.0f)
		});

	Entity entity8 = ecs.createEntity();
	ecs.addComponent(entity8, Renderable{
		"../modelfiles/Duck2.gltf",
		"../shaders/pbr.vert",
		"../shaders/water.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(entity8, Transform{
		glm::vec3(3.0f, 1.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
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
		"../../glTF-Sample-Models-master/2.0/Sponza/glTF/Sponza.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(entity5, Transform{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
		});

	Entity light1 = ecs.createEntity();
	ecs.addComponent(light1, Light{
		LightType::DIRECTIONAL,
		glm::vec3(0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec2(0.0f)
		});

	Entity light2 = ecs.createEntity();
	ecs.addComponent(light2, Light{
		LightType::SPOT,
		glm::vec3(5.0f, 4.0f, 0.0f),
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
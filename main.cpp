#include "src/Game.h"
#include "src/ecs/ECS.h"
#include "src/ecs/components/Transform.h"
#include "src/ecs/components/Camera.h"
#include "src/ecs/components/Renderable.h"
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

	Entity rectangle = ecs.createEntity();
	ecs.addComponent(rectangle, Renderable{
		"../modelfiles/BoomBox.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(rectangle, Transform{
		glm::vec3(5.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 90.0f, 0.0f),
		glm::vec3(100.0f, 100.0f, 100.0f)
		});

	Entity rectangle2 = ecs.createEntity();
	ecs.addComponent(rectangle2, Renderable{
		"../modelfiles/Duck2.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(rectangle2, Transform{
		glm::vec3(5.0f, 1.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.01f, 0.01f, 0.01f)
		});

	Entity rectangle3 = ecs.createEntity();
	ecs.addComponent(rectangle3, Renderable{
		"../modelfiles/DamagedHelmet.gltf",
		"../shaders/pbr.vert",
		"../shaders/pbr.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(rectangle3, Transform{
		glm::vec3(5.0f, 1.0f, -2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
		});

	g.launch();
}
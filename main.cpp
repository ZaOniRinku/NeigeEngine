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
		"../models/Box.gltf",
		"../shaders/dummy_shader.vert",
		"../shaders/dummy_shader.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(rectangle, Transform{
		glm::vec3(5.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 90.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
		});

	Entity rectangle2 = ecs.createEntity();
	ecs.addComponent(rectangle2, Renderable{
		"../models/Box.glb",
		"../shaders/dummy_shader.vert",
		"../shaders/dummy_shader.frag",
		"",
		"",
		"",
		Topology::TRIANGLE_LIST
		});
	ecs.addComponent(rectangle2, Transform{
		glm::vec3(5.0f, 1.0f, 2.0f),
		glm::vec3(180.0f, 180.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
		});

	g.launch();
}
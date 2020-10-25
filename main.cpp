#include "src/Game.h"
#include "src/ecs/ECS.h"
#include "src/ecs/components/Transform.h"
#include "src/ecs/components/Camera.h"

ECS ecs;

int main(void) {
	Window w;
	w.extent.width = 480;
	w.extent.height = 360;

	ecs.init();
	Entity entity = ecs.createEntity();
	ecs.registerComponent<Transform>();
	ecs.addComponent(entity, Transform{});
	ecs.registerComponent<Camera>();
	ecs.addComponent(entity, Camera{});

	Game g;
	g.window = &w;
	g.launch();
}
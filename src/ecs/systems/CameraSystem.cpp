#include "CameraSystem.h"
#include "../components/Camera.h"
#include "../../graphics/resources/ShaderResources.h"

extern ECS ecs;

void CameraSystem::init() {
	for (Entity entity : entities) {
		camera = entity;
	}
}
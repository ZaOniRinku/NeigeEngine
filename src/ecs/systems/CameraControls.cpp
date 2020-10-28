#include "CameraControls.h"
#include "../components/Transform.h"
#include "../../inputs/Inputs.h"

extern ECS ecs;

void CameraControls::update(double deltaTime) {
	for (Entity entity : entities) {
		auto& transform = ecs.getComponent<Transform>(entity);

		if (keyboardInputs.wKey == HELD) {
			transform.position.x += static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.aKey == HELD) {
			transform.position.z -= static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.sKey == HELD) {
			transform.position.x -= static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.dKey == HELD) {
			transform.position.z += static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.spaceKey == HELD) {
			transform.position.y += static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.shiftKey == HELD) {
			transform.position.y -= static_cast<float>(speed * deltaTime);
		}
	}
}

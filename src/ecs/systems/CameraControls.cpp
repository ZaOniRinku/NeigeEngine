#include "CameraControls.h"
#include "../components/Camera.h"
#include "../components/Transform.h"
#include "../../inputs/Inputs.h"

extern ECS ecs;

void CameraControls::update(double deltaTime) {
	for (Entity entity : entities) {
		auto& transform = ecs.getComponent<Transform>(entity);
		auto& camera = ecs.getComponent<Camera>(entity);

		if (keyboardInputs.wKey == KeyState::HELD) {
			transform.position += camera.to * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.aKey == KeyState::HELD) {
			transform.position -= glm::normalize(glm::cross(camera.to, glm::vec3(0.0f, 1.0f, 0.0f))) * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.sKey == KeyState::HELD) {
			transform.position -= camera.to * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.dKey == KeyState::HELD) {
			transform.position += glm::normalize(glm::cross(camera.to, glm::vec3(0.0f, 1.0f, 0.0f))) * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.spaceKey == KeyState::HELD) {
			transform.position.y += static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.shiftKey == KeyState::HELD) {
			transform.position.y -= static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.leftKey == KeyState::HELD) {
			angle -= static_cast<float>(sensitivity * deltaTime);
			camera.to.x = sin(angle);
			camera.to.z = -cos(angle);
		}
		if (keyboardInputs.rightKey == KeyState::HELD) {
			angle += static_cast<float>(sensitivity * deltaTime);
			camera.to.x = sin(angle);
			camera.to.z = -cos(angle);
		}
	}
}

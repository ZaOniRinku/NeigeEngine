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
			yaw -= static_cast<float>(sensitivity * deltaTime * 100.0f);
			yaw = glm::mod(yaw, 360.0f);
		}
		if (keyboardInputs.rightKey == KeyState::HELD) {
			yaw += static_cast<float>(sensitivity * deltaTime * 100.0f);
			yaw = glm::mod(yaw, 360.0f);
		}
		if (keyboardInputs.upKey == KeyState::HELD) {
			pitch -= static_cast<float>(sensitivity * deltaTime * 100.0f);
			pitch = std::clamp(pitch, -89.0f, 89.0f);
		}
		if (keyboardInputs.downKey == KeyState::HELD) {
			pitch += static_cast<float>(sensitivity * deltaTime * 100.0f);
			pitch = std::clamp(pitch, -89.0f, 89.0f);
		}
		camera.to.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		camera.to.y = -sin(glm::radians(pitch));
		camera.to.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		camera.to = glm::normalize(camera.to);
	}
}

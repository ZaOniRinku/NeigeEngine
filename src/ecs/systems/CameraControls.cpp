#include "CameraControls.h"
#include "../components/Camera.h"
#include "../../inputs/Inputs.h"
#include "../../graphics/resources/ShaderResources.h"

extern ECS ecs;

void CameraControls::update(double deltaTime) {
	for (Entity entity : entities) {
		auto& cameraCamera = ecs.getComponent<Camera>(entity);

		if (keyboardInputs.wKey == KeyState::HELD) {
			cameraCamera.position += cameraCamera.to * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.aKey == KeyState::HELD) {
			cameraCamera.position -= glm::normalize(glm::cross(cameraCamera.to, glm::vec3(0.0f, 1.0f, 0.0f))) * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.sKey == KeyState::HELD) {
			cameraCamera.position -= cameraCamera.to * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.dKey == KeyState::HELD) {
			cameraCamera.position += glm::normalize(glm::cross(cameraCamera.to, glm::vec3(0.0f, 1.0f, 0.0f))) * static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.spaceKey == KeyState::HELD) {
			cameraCamera.position.y += static_cast<float>(speed * deltaTime);
		}
		if (keyboardInputs.shiftKey == KeyState::HELD) {
			cameraCamera.position.y -= static_cast<float>(speed * deltaTime);
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
		cameraCamera.to.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraCamera.to.y = -sin(glm::radians(pitch));
		cameraCamera.to.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraCamera.to = glm::normalize(cameraCamera.to);
	}
}

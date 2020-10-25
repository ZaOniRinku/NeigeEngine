#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../../external/glm/glm/glm.hpp"
#include "../../../external/glm/glm/gtc/matrix_transform.hpp"

struct Camera {
	glm::mat4 lookAt;
	glm::mat4 perspectiveProjection;

	static glm::mat4 createLookAt(glm::vec3 position, glm::vec3 to, glm::vec3 up) {
		return glm::lookAt(position, to, up);
	}

	static glm::mat4 createPerspectiveProjection(float FOV, float aspectRatio, float nearPlane, float farPlane) {
		return glm::perspective(FOV, aspectRatio, nearPlane, farPlane);
	}

	static glm::mat4 createOrthoProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
		return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	}
};


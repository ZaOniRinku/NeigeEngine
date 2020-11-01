#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../../external/glm/glm/glm.hpp"
#include "../../../external/glm/glm/gtc/matrix_transform.hpp"

struct Camera {
	glm::vec3 position;
	glm::vec3 to;
	glm::mat4 projection;

	static glm::mat4 createLookAtView(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
		return glm::lookAt(eye, center, up);
	}

	static glm::mat4 createPerspectiveProjection(float FOV, float aspectRatio, float nearPlane, float farPlane) {
		glm::mat4 projection = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);
		projection[1][1] *= -1;
		return projection;
	}

	static glm::mat4 createOrthoProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
		return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	}
};


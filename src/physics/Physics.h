#pragma once
#include "../ecs/ECS.h"
#include "../../../external/glm/glm/glm.hpp"

struct Physics : public System {
	glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0);

	void update(double deltaTime);
};
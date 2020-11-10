#pragma once
#include "../../../external/glm/glm/glm.hpp"

struct Rigidbody {
	glm::vec3 velocity;
	glm::vec3 acceleration;
	bool affectedByGravity;
};
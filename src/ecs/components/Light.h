#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../../external/glm/glm/glm.hpp"
#include "../../utils/structs/ShaderStructs.h"

struct Light {
	LightType type;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	glm::vec2 cutoffs;
};
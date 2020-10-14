#pragma once
#include "vulkan/vulkan.hpp"
#include "Shader.h"
#include <vector>

class GraphicsPipeline {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<Shader> shaders;
};


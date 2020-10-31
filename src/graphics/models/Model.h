#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/resources/ModelLoader.h"
#include "../../utils/structs/ShaderStructs.h"
#include "../resources/Buffer.h"
#include "../commands/CommandBuffer.h"
#include <vector>

struct Model {
	std::vector<Primitive> primitives;
	Buffer vertexBuffer;
	Buffer indexBuffer;

	void init(std::string filePath);
	void destroy();
	void draw(CommandBuffer* commandBuffer);
};
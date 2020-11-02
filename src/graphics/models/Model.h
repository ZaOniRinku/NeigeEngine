#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/resources/ModelLoader.h"
#include "../../utils/structs/ShaderStructs.h"
#include "../resources/Buffer.h"
#include "../commands/CommandBuffer.h"
#include "../../utils/resources/BufferTools.h"
#include "../pipelines/DescriptorSet.h"
#include "../pipelines/GraphicsPipeline.h"
#include <vector>

struct Model {
	std::vector<Primitive> primitives;
	Buffer vertexBuffer;
	Buffer indexBuffer;
	std::vector<DescriptorSet> descriptorSets;

	void init(std::string filePath);
	void destroy();
	void draw(CommandBuffer* commandBuffer, bool bindTextures);
	void createDescriptorSets(GraphicsPipeline* graphicsPipeline);
};
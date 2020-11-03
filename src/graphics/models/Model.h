#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/resources/ModelLoader.h"
#include "../../utils/structs/ModelStructs.h"
#include "../../utils/structs/ShaderStructs.h"
#include "../commands/CommandBuffer.h"
#include "../../utils/resources/BufferTools.h"
#include "../pipelines/GraphicsPipeline.h"
#include "../../graphics/resources/Buffer.h"
#include <vector>

struct Model {
	std::vector<Mesh> meshes;
	Buffer vertexBuffer;
	Buffer indexBuffer;

	void init(std::string filePath);
	void destroy();
	void draw(CommandBuffer* commandBuffer, bool bindTextures);
	void createDescriptorSets(GraphicsPipeline* graphicsPipeline);
};
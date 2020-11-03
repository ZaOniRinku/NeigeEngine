#pragma once
#include "vulkan/vulkan.hpp"
#include "../../graphics/pipelines/DescriptorSet.h"
#include <vector>

// Model primitive
struct Primitive {
	uint32_t firstIndex;
	uint32_t indexCount;
	int32_t vertexOffset;
	uint64_t materialIndex;
};

// Model mesh
struct Mesh {
	uint32_t indexOffset;
	int32_t vertexOffset;
	std::vector<Primitive> primitives;
	std::vector<DescriptorSet> descriptorSets;
};
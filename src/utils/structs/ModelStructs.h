#pragma once
#include "vulkan/vulkan.hpp"
#include "../../external/glm/glm/glm.hpp"
#include "../../graphics/pipelines/DescriptorSet.h"
#include "../../graphics/resources/Buffer.h"
#include <vector>
#include <unordered_map>

// Model primitive
struct Primitive {
	uint32_t firstIndex;
	uint32_t indexCount;
	int32_t vertexOffset;
	uint64_t materialIndex;
};

// Mesh bone
struct Bone {
	glm::mat4 transformation;
	glm::mat4 inverseBindMatrix;
	std::vector<Bone> children;
};

// Model mesh
struct Mesh {
	uint32_t indexOffset;
	int32_t vertexOffset;
	std::vector<Primitive> primitives;
	Bone skeleton;
	std::vector<Bone> boneList;
	std::vector<Buffer> boneBuffers;
	std::unordered_map<std::string, std::vector<std::vector<DescriptorSet>>> descriptorSets;
};
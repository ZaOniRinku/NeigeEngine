#pragma once
#include "vulkan/vulkan.hpp"
#include "NeigeDefines.h"
#include <vector>
#include <iostream>
#include <algorithm>

// 256 MB
#define CHUNK_SIZE 268435456

struct Block {
	Block* next;

	VkDeviceSize offset;
	VkDeviceSize size;
	bool inUse;
};

struct Chunk {
	VkDeviceMemory memory;
	int32_t type;
	Block* head;

	Chunk(int32_t memoryType, VkDeviceSize size);
	VkDeviceSize allocate(VkMemoryRequirements memRequirements);
	void freeBlocks();
};

struct MemoryAllocator {
	std::vector<Chunk> chunks;

	void destroy();
	VkDeviceSize allocate(VkBuffer* bufferToAllocate, VkMemoryPropertyFlags flags);
	VkDeviceSize allocate(VkImage* imageToAllocate, VkMemoryPropertyFlags flags);
	int32_t findProperties(uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties);
};
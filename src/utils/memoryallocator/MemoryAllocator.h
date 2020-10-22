#pragma once
#include "vulkan/vulkan.hpp"
#include "../NeigeDefines.h"
#include <vector>
#include <iostream>
#include <algorithm>

// 256 MB
#define CHUNK_SIZE 268435456

struct Block {
	Block* prev;
	Block* next;

	VkDeviceSize offset;
	VkDeviceSize size;
	VkDeviceSize allocationId;
	bool inUse;
};

struct Chunk {
	VkDeviceMemory memory;
	int32_t type;
	Block* head;

	Chunk(int32_t memoryType, VkDeviceSize size);
	VkDeviceSize allocate(VkMemoryRequirements memRequirements, VkDeviceSize* allocationNumber);
	void freeBlocks();
};

struct MemoryAllocator {
	std::vector<Chunk> chunks;
	VkDeviceSize allocationNumber;

	void destroy();
	VkDeviceSize allocate(VkBuffer* bufferToAllocate, VkMemoryPropertyFlags flags);
	VkDeviceSize allocate(VkImage* imageToAllocate, VkMemoryPropertyFlags flags);
	void deallocate(VkDeviceSize allocationId);
	int32_t findProperties(uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties);
};
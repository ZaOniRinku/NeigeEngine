#pragma once
#include <vulkan/vulkan.hpp>
#include "NeigeTools.h"
#include "../graphics/LogicalDevice.h"
#include "../graphics/PhysicalDevice.h"
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

	Chunk(VkDevice device, int32_t memoryType, VkDeviceSize size) {
		type = memoryType;

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = size;
		allocInfo.memoryTypeIndex = memoryType;
		NEIGE_VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &memory));

		Block* block = new Block();
		block->offset = 0;
		block->size = size;
		block->inUse = false;
		block->next = nullptr;

		head = block;
	}

	VkDeviceSize allocate(VkMemoryRequirements memRequirements) {
		Block* curr = head;
		while (curr) {
			if (!curr->inUse) {
				VkDeviceSize actualSize = curr->size;

				// Size of the block minus the alignment (wasted bytes)
				if (curr->offset % memRequirements.alignment != 0) {
					actualSize -= memRequirements.alignment - curr->offset % memRequirements.alignment;
				}

				// If we have enough space to allocate here
				if (actualSize >= memRequirements.size) {
					// Set the size of the block to be the size with alignment
					curr->size = actualSize;

					// Align offset
					if (curr->offset % memRequirements.alignment != 0) {
						curr->offset += memRequirements.alignment - curr->offset % memRequirements.alignment;
					}

					// If we have exactly enough space, no need to subdivide
					if (curr->size == memRequirements.size) {
						curr->inUse = true;
						return curr->offset;
					}

					// Subdivide the block by creating a new one
					Block* newBlock = new Block();
					newBlock->inUse = false;
					newBlock->offset = curr->offset + memRequirements.size;
					newBlock->size = curr->size - memRequirements.size;
					newBlock->next = curr->next;

					// The size of the current block is now the size of the data
					curr->size = memRequirements.size;
					curr->inUse = true;
					curr->next = newBlock;

					return curr->offset;
				}
			}
			curr = curr->next;
		}

		return -1;
	}

	void freeBlocks() {
		Block* curr;
		while (head) {
			curr = head;
			head = head->next;
			delete curr;
		}
	}
};

struct MemoryAllocator {
	VkDeviceSize allocate(LogicalDevice* logicalDevice, PhysicalDevice* physicalDevice, VkBuffer* bufferToAllocate, VkMemoryPropertyFlags flags) {
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice->device, *bufferToAllocate, &memRequirements);
		int32_t properties = findProperties(physicalDevice, memRequirements.memoryTypeBits, flags);

		// Look for the first block with enough space
		for (Chunk chunk : chunks) {
			if (chunk.type == properties) {
				VkDeviceSize offset;
				offset = chunk.allocate(memRequirements);

				if (offset != -1) {
					vkBindBufferMemory(logicalDevice->device, *bufferToAllocate, chunk.memory, offset);
					return 1;
				}
			}
		}

		// No block has been found, create a new chunk
		Chunk newChunk = Chunk(logicalDevice->device, properties, std::max((VkDeviceSize)CHUNK_SIZE, memRequirements.size));

		// Add to this chunk
		VkDeviceSize offset;
		offset = newChunk.allocate(memRequirements);

		if (offset == -1) {
			NEIGE_ERROR("Unable to allocate memory (buffer).");
		}

		vkBindBufferMemory(logicalDevice->device, *bufferToAllocate, newChunk.memory, offset);

		chunks.push_back(newChunk);

		return 1;
	}

	VkDeviceSize allocate(LogicalDevice* logicalDevice, PhysicalDevice* physicalDevice, VkImage* imageToAllocate, VkMemoryPropertyFlags flags) {
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(logicalDevice->device, *imageToAllocate, &memRequirements);
		int32_t properties = findProperties(physicalDevice, memRequirements.memoryTypeBits, flags);

		// Look for the first block with enough space
		for (Chunk chunk : chunks) {
			if (chunk.type == properties) {
				VkDeviceSize offset;
				offset = chunk.allocate(memRequirements);

				if (offset != -1) {
					vkBindImageMemory(logicalDevice->device, *imageToAllocate, chunk.memory, offset);
					return 1;
				}
			}
		}

		// No block has been found, create a new chunk
		Chunk newChunk = Chunk(logicalDevice->device, properties, std::max((VkDeviceSize)CHUNK_SIZE, memRequirements.size));

		// Add to this chunk
		VkDeviceSize offset;
		offset = newChunk.allocate(memRequirements);

		if (offset == -1) {
			NEIGE_ERROR("Unable to allocate memory (image).");
		}

		vkBindImageMemory(logicalDevice->device, *imageToAllocate, newChunk.memory, offset);

		chunks.push_back(newChunk);

		return 1;
	}

	void free(LogicalDevice* logicalDevice) {
		for (Chunk chunk : chunks) {
			chunk.freeBlocks();
			vkFreeMemory(logicalDevice->device, chunk.memory, nullptr);
		}
	}

	int32_t findProperties(PhysicalDevice* physicalDevice, uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties) {
		const uint32_t memoryCount = physicalDevice->memoryProperties.memoryTypeCount;
		for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; memoryIndex++) {
			const uint32_t memoryTypeBits = (1 << memoryIndex);
			const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;

			const VkMemoryPropertyFlags properties = physicalDevice->memoryProperties.memoryTypes[memoryIndex].propertyFlags;
			const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

			if (isRequiredMemoryType && hasRequiredProperties) {
				return static_cast<int32_t>(memoryIndex);
			}
		}
		NEIGE_ERROR("Unable to find suitable memory type.");
	}
	
	std::vector<Chunk> chunks;
};
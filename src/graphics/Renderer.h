#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"
#include "../utils/ImageTools.h"
#include "../utils/PhysicalDevicePicker.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "GraphicsPipeline.h"
#include "Image.h"
#include "Semaphore.h"
#include "../window/Window.h"
#include <iostream>
#include <vector>
#include <string>

#define MAX_FRAMES_IN_FLIGHT 3

struct Renderer {
	Window* window;

	std::vector<Fence> fences;
	std::vector<Semaphore> IAsemaphores;
	std::vector<Semaphore> RFsemaphores;

	std::vector<Image> colorImages;
	std::vector<Image> depthImages;
	std::vector<RenderPass> renderPasses;
	std::vector<Framebuffer> framebuffers;
	std::vector<GraphicsPipeline> graphicsPipelines;
	std::vector<CommandPool> renderingCommandPools;
	std::vector<CommandBuffer> renderingCommandBuffers;
	Buffer vertexBuffer;
	Buffer indexBuffer;

	uint32_t swapchainSize;
	uint32_t currentFrame = 0;

	void init();
	void update();
	void destroy();
	void recordRenderingCommands(uint32_t frameInFlightIndex, uint32_t framebufferIndex);
	void createResources();
	void destroyResources();
};
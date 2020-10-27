#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/structs/RendererStructs.h"
#include "../utils/structs/ShaderStructs.h"
#include "../utils/NeigeVKTranslate.h"
#include "../utils/resources/ImageTools.h"
#include "devices/PhysicalDevicePicker.h"
#include "commands/CommandBuffer.h"
#include "commands/CommandPool.h"
#include "pipelines/GraphicsPipeline.h"
#include "pipelines/Shader.h"
#include "pipelines/Viewport.h"
#include "resources/Image.h"
#include "renderpasses/Framebuffer.h"
#include "renderpasses/RenderPass.h"
#include "sync/Fence.h"
#include "sync/Semaphore.h"
#include "../window/Window.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>

#define MAX_FRAMES_IN_FLIGHT 1

struct Renderer {
	Window* window;

	std::vector<Fence> fences;
	std::vector<Semaphore> IAsemaphores;
	std::vector<Semaphore> RFsemaphores;

	Viewport fullscreenViewport;

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

	bool pressed = false;

	void init();
	void update();
	void destroy();
	void recordRenderingCommands(uint32_t frameInFlightIndex, uint32_t framebufferIndex);
	void createResources();
	void destroyResources();
};
#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "../../utils/structs/RendererStructs.h"
#include "../../utils/structs/ShaderStructs.h"
#include "../../utils/NeigeVKTranslate.h"
#include "../../utils/resources/ImageTools.h"
#include "../../graphics/devices/PhysicalDevicePicker.h"
#include "../../graphics/commands/CommandBuffer.h"
#include "../../graphics/commands/CommandPool.h"
#include "../../graphics/pipelines/GraphicsPipeline.h"
#include "../../graphics/pipelines/Shader.h"
#include "../../graphics/pipelines/Viewport.h"
#include "../../graphics/resources/Image.h"
#include "../../graphics/renderpasses/Framebuffer.h"
#include "../../graphics/renderpasses/RenderPass.h"
#include "../../graphics/sync/Fence.h"
#include "../../graphics/sync/Semaphore.h"
#include "../../window/Window.h"
#include "../ECS.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>

#define MAX_FRAMES_IN_FLIGHT 1

struct Renderer : public System {
	Window* window;

	// Entities
	Entity camera;

	// Sync
	std::vector<Fence> fences;
	std::vector<Semaphore> IAsemaphores;
	std::vector<Semaphore> RFsemaphores;

	// Pipelines
	Viewport fullscreenViewport;
	std::unordered_map<std::string, GraphicsPipeline> graphicsPipelines;

	// Render passes
	std::vector<Image> colorImages;
	std::vector<Image> depthImages;
	std::vector<RenderPass> renderPasses;
	std::vector<Framebuffer> framebuffers;
	std::vector<CommandPool> renderingCommandPools;
	std::vector<CommandBuffer> renderingCommandBuffers;

	// Buffers
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
#pragma once
#include "vulkan/vulkan.hpp"
#include "../../../external/glm/glm/glm.hpp"
#include "../commands/CommandBuffer.h"
#include "../resources/Buffer.h"
#include "../resources/Image.h"
#include "../renderpasses/Framebuffer.h"
#include "../pipelines/DescriptorSet.h"
#include "../pipelines/GraphicsPipeline.h"
#include "../pipelines/Viewport.h"
#include <numeric>
#include <vector>
#include <random>

#define DOWNSCALE 2
#define SSAOSAMPLES 64

struct SSAO {
	Viewport viewport;
	RenderPass colorRenderPass;

	// Depth to positions and normals
	GraphicsPipeline depthToPositionsGraphicsPipeline;
	GraphicsPipeline depthToNormalsGraphicsPipeline;
	std::vector<DescriptorSet> depthToPositionsAndNormalsDescriptorSets;
	Image depthToPositionsImage;
	Image depthToNormalsImage;
	std::vector<Framebuffer> depthToPositionsFramebuffers;
	std::vector<Framebuffer> depthToNormalsFramebuffers;

	// Sample kernel
	Buffer sampleKernel;
	Image randomTexture;

	// SSAO
	GraphicsPipeline ssaoGraphicsPipeline;
	std::vector<DescriptorSet> ssaoDescriptorSets;
	Image ssaoImage;
	std::vector<Framebuffer> ssaoFramebuffers;

	// SSAO blurred
	GraphicsPipeline ssaoBlurredGraphicsPipeline;
	std::vector<DescriptorSet> ssaoBlurredDescriptorSets;
	Image ssaoBlurredImage;
	std::vector<Framebuffer> ssaoBlurredFramebuffers;

	void init(Viewport fullscreenViewport);
	void destroy();
	void createResources(Viewport fullscreenViewport);
	void destroyResources();
	void createRandomTexture();
	void draw(CommandBuffer* commandBuffer, uint32_t frameInFlightIndex);
};
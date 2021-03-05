#pragma once
#include "vulkan/vulkan.hpp"
#include "../../../../external/glm/glm/glm.hpp"
#include "../../resources/Buffer.h"
#include "../../resources/Image.h"
#include "../../renderpasses/Framebuffer.h"
#include "../../renderpasses/RenderPass.h"
#include "../../pipelines/GraphicsPipeline.h"
#include "../../pipelines/Viewport.h"
#include <numeric>
#include <vector>
#include <random>

struct DepthPrepass {
	Viewport viewport;
	RenderPass renderPass;
	GraphicsPipeline graphicsPipeline;
	Image image;
	std::vector<Framebuffer> framebuffers;

	void init(Viewport fullscreenViewport);
	void destroy();
	void createResources(Viewport fullscreenViewport);
	void destroyResources();
};
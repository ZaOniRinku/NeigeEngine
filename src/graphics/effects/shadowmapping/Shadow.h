#pragma once
#include "vulkan/vulkan.hpp"
#include "../../../utils/structs/RendererStructs.h"
#include "../../pipelines/Viewport.h"
#include "../../pipelines/GraphicsPipeline.h"
#include "../../resources/Buffer.h"
#include "../../resources/Image.h"
#include "../../renderpasses/Framebuffer.h"
#include "../../renderpasses/RenderPass.h"
#include "../../../utils/resources/BufferTools.h"
#include "../../../utils/resources/ImageTools.h"

struct Shadow {
	Viewport viewport;
	GraphicsPipeline graphicsPipeline;
	std::vector<Buffer> buffers;
	std::vector<Image> images;
	RenderPass renderPass;
	std::vector<std::vector<Framebuffer>> framebuffers;
	Image defaultShadow;
	int mapCount;

	void init();
	void destroy();
};
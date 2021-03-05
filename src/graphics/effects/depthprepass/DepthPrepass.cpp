#include "DepthPrepass.h"
#include "../../../utils/resources/BufferTools.h"
#include "../../../utils/resources/ImageTools.h"
#include "../../../graphics/resources/RendererResources.h"
#include "../../../graphics/resources/ShaderResources.h"

void DepthPrepass::init(Viewport fullscreenViewport) {
	viewport.init(static_cast<uint32_t>(fullscreenViewport.viewport.width), static_cast<uint32_t>(fullscreenViewport.viewport.height));

	{
		std::vector<RenderPassAttachment> attachments;
		attachments.push_back(RenderPassAttachment(AttachmentType::DEPTH, physicalDevice.depthFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));

		std::vector<SubpassDependency> dependencies;
		dependencies.push_back({ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

		renderPass.init(attachments, dependencies);
	}

	graphicsPipeline.vertexShaderPath = "../shaders/depthPrepass.vert";
	graphicsPipeline.renderPass = &renderPass;
	graphicsPipeline.viewport = &viewport;
	graphicsPipeline.colorBlend = false;
	graphicsPipeline.multiSample = false;
	graphicsPipeline.backfaceCulling = true;
	graphicsPipeline.init();

	createResources(fullscreenViewport);
}

void DepthPrepass::destroy() {
	renderPass.destroy();
	graphicsPipeline.destroy();
	destroyResources();
}

void DepthPrepass::createResources(Viewport fullscreenViewport) {
	viewport.init(static_cast<uint32_t>(fullscreenViewport.viewport.width), static_cast<uint32_t>(fullscreenViewport.viewport.height));
	
	// Image
	ImageTools::createImage(&image.image, 1, static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image.allocationId);
	ImageTools::createImageView(&image.imageView, image.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	ImageTools::createImageSampler(&image.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	ImageTools::transitionLayout(image.image, physicalDevice.colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);

	// Framebuffer
	framebuffers.resize(MAX_FRAMES_IN_FLIGHT);

	{
		std::vector<std::vector<VkImageView>> framebufferAttachements;
		framebufferAttachements.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			framebufferAttachements[i].push_back(image.imageView);
			framebuffers[i].init(&renderPass, framebufferAttachements[i], static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1);
		}
	}
}

void DepthPrepass::destroyResources() {
	image.destroy();
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.destroy();
	}
	framebuffers.clear();
	framebuffers.shrink_to_fit();
}
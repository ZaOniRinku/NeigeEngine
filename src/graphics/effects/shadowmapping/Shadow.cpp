#include "Shadow.h"
#include "../../../graphics/resources/RendererResources.h"

void Shadow::init() {
	viewport.init(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

	buffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (Buffer& buffer : buffers) {
		BufferTools::createUniformBuffer(buffer.buffer, buffer.deviceMemory, sizeof(ShadowUniformBufferObject));
	}

	std::vector<RenderPassAttachment> attachments;
	attachments.push_back(RenderPassAttachment(AttachmentType::DEPTH, physicalDevice.depthFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));

	std::vector<SubpassDependency> dependencies;
	dependencies.push_back({ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT });
	dependencies.push_back({ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

	renderPass.init(attachments, dependencies);

	ImageTools::createImage(&defaultShadow.image, 1, 1, 1, 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &defaultShadow.allocationId);
	ImageTools::createImageView(&defaultShadow.imageView, defaultShadow.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	ImageTools::createImageSampler(&defaultShadow.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE, VK_COMPARE_OP_LESS);
	ImageTools::transitionLayout(defaultShadow.image, physicalDevice.depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 1, 1);

	graphicsPipeline.vertexShaderPath = "../shaders/shadow.vert";
	graphicsPipeline.renderPass = &renderPass;
	graphicsPipeline.viewport = &viewport;
	graphicsPipeline.colorBlend = false;
	graphicsPipeline.multiSample = false;
	graphicsPipeline.init();
}

void Shadow::destroy() {
	renderPass.destroy();
	graphicsPipeline.destroy();
	for (Buffer& buffer : buffers) {
		buffer.destroy();
	}
	defaultShadow.destroy();
	for (Image& image : images) {
		image.destroy();
	}
	for (size_t i = 0; i < framebuffers.size(); i++) {
		for (Framebuffer& framebuffer : framebuffers[i]) {
			framebuffer.destroy();
		}
	}
}

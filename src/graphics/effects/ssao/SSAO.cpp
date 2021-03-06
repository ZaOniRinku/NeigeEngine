#include "SSAO.h"
#include "../../../utils/resources/BufferTools.h"
#include "../../../utils/resources/ImageTools.h"
#include "../../../graphics/resources/RendererResources.h"
#include "../../../graphics/resources/ShaderResources.h"

void SSAO::init(Viewport fullscreenViewport) {
	viewport.init(static_cast<uint32_t>(fullscreenViewport.viewport.width) / DOWNSCALE, static_cast<uint32_t>(fullscreenViewport.viewport.height) / DOWNSCALE);

	{
		std::vector<RenderPassAttachment> attachments;
		attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, physicalDevice.colorFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

		std::vector<SubpassDependency> dependencies;
		dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });
		
		colorRenderPass.init(attachments, dependencies);
	}
	
	depthToNormalsGraphicsPipeline.vertexShaderPath = "../shaders/fullscreenTriangle.vert";
	depthToNormalsGraphicsPipeline.fragmentShaderPath = "../shaders/depthToNormals.frag";
	depthToNormalsGraphicsPipeline.renderPass = &colorRenderPass;
	depthToNormalsGraphicsPipeline.viewport = &viewport;
	depthToNormalsGraphicsPipeline.colorBlend = false;
	depthToNormalsGraphicsPipeline.multiSample = false;
	depthToNormalsGraphicsPipeline.backfaceCulling = false;
	depthToNormalsGraphicsPipeline.init();

	depthToPositionsGraphicsPipeline.vertexShaderPath = "../shaders/fullscreenTriangle.vert";
	depthToPositionsGraphicsPipeline.fragmentShaderPath = "../shaders/depthToPositions.frag";
	depthToPositionsGraphicsPipeline.renderPass = &colorRenderPass;
	depthToPositionsGraphicsPipeline.viewport = &viewport;
	depthToPositionsGraphicsPipeline.colorBlend = false;
	depthToPositionsGraphicsPipeline.multiSample = false;
	depthToPositionsGraphicsPipeline.backfaceCulling = false;
	depthToPositionsGraphicsPipeline.init();

	ssaoGraphicsPipeline.vertexShaderPath = "../shaders/fullscreenTriangle.vert";
	ssaoGraphicsPipeline.fragmentShaderPath = "../shaders/ssao.frag";
	ssaoGraphicsPipeline.renderPass = &colorRenderPass;
	ssaoGraphicsPipeline.viewport = &viewport;
	ssaoGraphicsPipeline.colorBlend = false;
	ssaoGraphicsPipeline.multiSample = false;
	ssaoGraphicsPipeline.backfaceCulling = false;
	ssaoGraphicsPipeline.init();

	ssaoBlurredGraphicsPipeline.vertexShaderPath = "../shaders/fullscreenTriangle.vert";
	ssaoBlurredGraphicsPipeline.fragmentShaderPath = "../shaders/ssaoBlur.frag";
	ssaoBlurredGraphicsPipeline.renderPass = &colorRenderPass;
	ssaoBlurredGraphicsPipeline.viewport = &viewport;
	ssaoBlurredGraphicsPipeline.colorBlend = false;
	ssaoBlurredGraphicsPipeline.multiSample = false;
	ssaoBlurredGraphicsPipeline.backfaceCulling = false;
	ssaoBlurredGraphicsPipeline.init();

	BufferTools::createUniformBuffer(sampleKernel.buffer, sampleKernel.deviceMemory, SSAOSAMPLES * 4 * sizeof(float));

	createRandomTexture();

	createResources(fullscreenViewport);
}

void SSAO::destroy() {
	destroyResources();
	sampleKernel.destroy();
	randomTexture.destroy();
	colorRenderPass.destroy();
	depthToNormalsGraphicsPipeline.destroy();
	depthToPositionsGraphicsPipeline.destroy();
	ssaoGraphicsPipeline.destroy();
	ssaoBlurredGraphicsPipeline.destroy();
}

void SSAO::createResources(Viewport fullscreenViewport) {
	viewport.init(static_cast<uint32_t>(fullscreenViewport.viewport.width) / DOWNSCALE, static_cast<uint32_t>(fullscreenViewport.viewport.height) / DOWNSCALE);
	
	// Images
	ImageTools::createImage(&depthToPositionsImage.image, 1, static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthToPositionsImage.allocationId);
	ImageTools::createImageView(&depthToPositionsImage.imageView, depthToPositionsImage.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&depthToPositionsImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	ImageTools::transitionLayout(depthToPositionsImage.image, physicalDevice.colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);

	ImageTools::createImage(&depthToNormalsImage.image, 1, static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthToNormalsImage.allocationId);
	ImageTools::createImageView(&depthToNormalsImage.imageView, depthToNormalsImage.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&depthToNormalsImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	ImageTools::transitionLayout(depthToNormalsImage.image, physicalDevice.colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);

	ImageTools::createImage(&ssaoImage.image, 1, static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ssaoImage.allocationId);
	ImageTools::createImageView(&ssaoImage.imageView, ssaoImage.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&ssaoImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	ImageTools::transitionLayout(ssaoImage.image, physicalDevice.colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);

	ImageTools::createImage(&ssaoBlurredImage.image, 1, static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ssaoBlurredImage.allocationId);
	ImageTools::createImageView(&ssaoBlurredImage.imageView, ssaoBlurredImage.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&ssaoBlurredImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	ImageTools::transitionLayout(ssaoBlurredImage.image, physicalDevice.colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);

	// Framebuffers
	depthToPositionsFramebuffers.resize(MAX_FRAMES_IN_FLIGHT);
	depthToNormalsFramebuffers.resize(MAX_FRAMES_IN_FLIGHT);
	ssaoFramebuffers.resize(MAX_FRAMES_IN_FLIGHT);
	ssaoBlurredFramebuffers.resize(MAX_FRAMES_IN_FLIGHT);

	{
		std::vector<std::vector<VkImageView>> framebufferAttachements;
		framebufferAttachements.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			framebufferAttachements[i].push_back(depthToPositionsImage.imageView);
			depthToPositionsFramebuffers[i].init(&colorRenderPass, framebufferAttachements[i], static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1);
		}
	}

	{
		std::vector<std::vector<VkImageView>> framebufferAttachements;
		framebufferAttachements.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			framebufferAttachements[i].push_back(depthToNormalsImage.imageView);
			depthToNormalsFramebuffers[i].init(&colorRenderPass, framebufferAttachements[i], static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1);
		}
	}

	{
		std::vector<std::vector<VkImageView>> framebufferAttachements;
		framebufferAttachements.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			framebufferAttachements[i].push_back(ssaoImage.imageView);
			ssaoFramebuffers[i].init(&colorRenderPass, framebufferAttachements[i], static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1);
		}
	}

	{
		std::vector<std::vector<VkImageView>> framebufferAttachements;
		framebufferAttachements.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			framebufferAttachements[i].push_back(ssaoBlurredImage.imageView);
			ssaoBlurredFramebuffers[i].init(&colorRenderPass, framebufferAttachements[i], static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height), 1);
		}
	}

	// Descriptor sets
	{
		depthToPositionsAndNormalsDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			depthToPositionsAndNormalsDescriptorSets[i].init(&depthToNormalsGraphicsPipeline, 0);

			VkDescriptorImageInfo depthPrepassInfo = {};
			depthPrepassInfo.sampler = depthPrepass.image.imageSampler;
			depthPrepassInfo.imageView = depthPrepass.image.imageView;
			depthPrepassInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			VkDescriptorBufferInfo cameraInfo = {};
			cameraInfo.buffer = cameraBuffers[i].buffer;
			cameraInfo.offset = 0;
			cameraInfo.range = sizeof(CameraUniformBufferObject);

			std::vector<VkWriteDescriptorSet> writesDescriptorSet;

			VkWriteDescriptorSet depthPrepassWriteDescriptorSet = {};
			depthPrepassWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			depthPrepassWriteDescriptorSet.pNext = nullptr;
			depthPrepassWriteDescriptorSet.dstSet = depthToPositionsAndNormalsDescriptorSets[i].descriptorSet;
			depthPrepassWriteDescriptorSet.dstBinding = 0;
			depthPrepassWriteDescriptorSet.dstArrayElement = 0;
			depthPrepassWriteDescriptorSet.descriptorCount = 1;
			depthPrepassWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			depthPrepassWriteDescriptorSet.pImageInfo = &depthPrepassInfo;
			depthPrepassWriteDescriptorSet.pBufferInfo = nullptr;
			depthPrepassWriteDescriptorSet.pTexelBufferView = nullptr;
			writesDescriptorSet.push_back(depthPrepassWriteDescriptorSet);

			VkWriteDescriptorSet cameraWriteDescriptorSet = {};
			cameraWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			cameraWriteDescriptorSet.pNext = nullptr;
			cameraWriteDescriptorSet.dstSet = depthToPositionsAndNormalsDescriptorSets[i].descriptorSet;
			cameraWriteDescriptorSet.dstBinding = 1;
			cameraWriteDescriptorSet.dstArrayElement = 0;
			cameraWriteDescriptorSet.descriptorCount = 1;
			cameraWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			cameraWriteDescriptorSet.pImageInfo = nullptr;
			cameraWriteDescriptorSet.pBufferInfo = &cameraInfo;
			cameraWriteDescriptorSet.pTexelBufferView = nullptr;
			writesDescriptorSet.push_back(cameraWriteDescriptorSet);

			depthToPositionsAndNormalsDescriptorSets[i].update(writesDescriptorSet);
		}
	}

	{
		ssaoDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			ssaoDescriptorSets[i].init(&ssaoGraphicsPipeline, 0);

			VkDescriptorImageInfo positionInfo = {};
			positionInfo.sampler = depthToPositionsImage.imageSampler;
			positionInfo.imageView = depthToPositionsImage.imageView;
			positionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkDescriptorImageInfo normalInfo = {};
			normalInfo.sampler = depthToNormalsImage.imageSampler;
			normalInfo.imageView = depthToNormalsImage.imageView;
			normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkDescriptorImageInfo randomTextureInfo = {};
			randomTextureInfo.sampler = randomTexture.imageSampler;
			randomTextureInfo.imageView = randomTexture.imageView;
			randomTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkDescriptorBufferInfo sampleKernelInfo = {};
			sampleKernelInfo.buffer = sampleKernel.buffer;
			sampleKernelInfo.offset = 0;
			sampleKernelInfo.range = SSAOSAMPLES * 4 * sizeof(float);

			VkDescriptorBufferInfo cameraInfo = {};
			cameraInfo.buffer = cameraBuffers[i].buffer;
			cameraInfo.offset = 0;
			cameraInfo.range = sizeof(CameraUniformBufferObject);

			std::vector<VkWriteDescriptorSet> writesDescriptorSet;

			VkWriteDescriptorSet positionWriteDescriptorSet = {};
			positionWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			positionWriteDescriptorSet.pNext = nullptr;
			positionWriteDescriptorSet.dstSet = ssaoDescriptorSets[i].descriptorSet;
			positionWriteDescriptorSet.dstBinding = 0;
			positionWriteDescriptorSet.dstArrayElement = 0;
			positionWriteDescriptorSet.descriptorCount = 1;
			positionWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			positionWriteDescriptorSet.pImageInfo = &positionInfo;
			positionWriteDescriptorSet.pBufferInfo = nullptr;
			positionWriteDescriptorSet.pTexelBufferView = nullptr;
			writesDescriptorSet.push_back(positionWriteDescriptorSet);

			VkWriteDescriptorSet normalWriteDescriptorSet = {};
			normalWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			normalWriteDescriptorSet.pNext = nullptr;
			normalWriteDescriptorSet.dstSet = ssaoDescriptorSets[i].descriptorSet;
			normalWriteDescriptorSet.dstBinding = 1;
			normalWriteDescriptorSet.dstArrayElement = 0;
			normalWriteDescriptorSet.descriptorCount = 1;
			normalWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			normalWriteDescriptorSet.pImageInfo = &normalInfo;
			normalWriteDescriptorSet.pBufferInfo = nullptr;
			normalWriteDescriptorSet.pTexelBufferView = nullptr;
			writesDescriptorSet.push_back(normalWriteDescriptorSet);

			VkWriteDescriptorSet randomTextureWriteDescriptorSet = {};
			randomTextureWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			randomTextureWriteDescriptorSet.pNext = nullptr;
			randomTextureWriteDescriptorSet.dstSet = ssaoDescriptorSets[i].descriptorSet;
			randomTextureWriteDescriptorSet.dstBinding = 2;
			randomTextureWriteDescriptorSet.dstArrayElement = 0;
			randomTextureWriteDescriptorSet.descriptorCount = 1;
			randomTextureWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			randomTextureWriteDescriptorSet.pImageInfo = &randomTextureInfo;
			randomTextureWriteDescriptorSet.pBufferInfo = nullptr;
			randomTextureWriteDescriptorSet.pTexelBufferView = nullptr;
			writesDescriptorSet.push_back(randomTextureWriteDescriptorSet);

			VkWriteDescriptorSet sampleKernelWriteDescriptorSet = {};
			sampleKernelWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			sampleKernelWriteDescriptorSet.pNext = nullptr;
			sampleKernelWriteDescriptorSet.dstSet = ssaoDescriptorSets[i].descriptorSet;
			sampleKernelWriteDescriptorSet.dstBinding = 3;
			sampleKernelWriteDescriptorSet.dstArrayElement = 0;
			sampleKernelWriteDescriptorSet.descriptorCount = 1;
			sampleKernelWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			sampleKernelWriteDescriptorSet.pImageInfo = nullptr;
			sampleKernelWriteDescriptorSet.pBufferInfo = &sampleKernelInfo;
			sampleKernelWriteDescriptorSet.pTexelBufferView = nullptr;
			writesDescriptorSet.push_back(sampleKernelWriteDescriptorSet);

			VkWriteDescriptorSet cameraWriteDescriptorSet = {};
			cameraWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			cameraWriteDescriptorSet.pNext = nullptr;
			cameraWriteDescriptorSet.dstSet = ssaoDescriptorSets[i].descriptorSet;
			cameraWriteDescriptorSet.dstBinding = 4;
			cameraWriteDescriptorSet.dstArrayElement = 0;
			cameraWriteDescriptorSet.descriptorCount = 1;
			cameraWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			cameraWriteDescriptorSet.pImageInfo = nullptr;
			cameraWriteDescriptorSet.pBufferInfo = &cameraInfo;
			cameraWriteDescriptorSet.pTexelBufferView = nullptr;
			writesDescriptorSet.push_back(cameraWriteDescriptorSet);

			ssaoDescriptorSets[i].update(writesDescriptorSet);
		}

		{
			ssaoBlurredDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				ssaoBlurredDescriptorSets[i].init(&ssaoBlurredGraphicsPipeline, 0);

				VkDescriptorImageInfo ssaoInfo = {};
				ssaoInfo.sampler = ssaoImage.imageSampler;
				ssaoInfo.imageView = ssaoImage.imageView;
				ssaoInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				std::vector<VkWriteDescriptorSet> writesDescriptorSet;

				VkWriteDescriptorSet ssaoWriteDescriptorSet = {};
				ssaoWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				ssaoWriteDescriptorSet.pNext = nullptr;
				ssaoWriteDescriptorSet.dstSet = ssaoBlurredDescriptorSets[i].descriptorSet;
				ssaoWriteDescriptorSet.dstBinding = 0;
				ssaoWriteDescriptorSet.dstArrayElement = 0;
				ssaoWriteDescriptorSet.descriptorCount = 1;
				ssaoWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				ssaoWriteDescriptorSet.pImageInfo = &ssaoInfo;
				ssaoWriteDescriptorSet.pBufferInfo = nullptr;
				ssaoWriteDescriptorSet.pTexelBufferView = nullptr;
				writesDescriptorSet.push_back(ssaoWriteDescriptorSet);

				ssaoBlurredDescriptorSets[i].update(writesDescriptorSet);
			}
		}
	}
}

void SSAO::destroyResources() {
	depthToPositionsImage.destroy();
	depthToNormalsImage.destroy();
	ssaoImage.destroy();
	ssaoBlurredImage.destroy();
	for (Framebuffer& framebuffer : depthToPositionsFramebuffers) {
		framebuffer.destroy();
	}
	depthToPositionsFramebuffers.clear();
	depthToPositionsFramebuffers.shrink_to_fit();
	for (Framebuffer& framebuffer : depthToNormalsFramebuffers) {
		framebuffer.destroy();
	}
	depthToNormalsFramebuffers.clear();
	depthToNormalsFramebuffers.shrink_to_fit();
	for (Framebuffer& framebuffer : ssaoFramebuffers) {
		framebuffer.destroy();
	}
	ssaoFramebuffers.clear();
	ssaoFramebuffers.shrink_to_fit();
	for (Framebuffer& framebuffer : ssaoBlurredFramebuffers) {
		framebuffer.destroy();
	}
	ssaoBlurredFramebuffers.clear();
	ssaoBlurredFramebuffers.shrink_to_fit();
}

void SSAO::createRandomTexture() {
	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine gen;

	std::vector<glm::vec4> samples;
	float randomRotations[64];

	for (unsigned char i = 0; i < SSAOSAMPLES; i++) {
		// Sample kernel
		glm::vec3 sample{ randomFloats(gen) * 2.0f - 1.0f,
		randomFloats(gen) * 2.0f - 1.0f,
		randomFloats(gen) };
		sample = glm::normalize(sample);
		sample *= randomFloats(gen);

		float scale = static_cast<float>(i) / 64.0f;
		// Lerp
		scale = 0.1f + (scale * scale) * (1.0f - 0.1f);

		sample *= scale;

		samples.push_back(glm::vec4(sample, 0.0f));
	}

	VkDeviceSize size = SSAOSAMPLES * 4 * sizeof(float);
	void* data;
	sampleKernel.map(0, size, &data);
	memcpy(data, samples.data(), size);
	sampleKernel.unmap();

	for (unsigned char i = 0; i < 16; i++) {
		// Random rotation
		randomRotations[(i * 4) + 0] = randomFloats(gen) * 2.0f - 1.0f;
		randomRotations[(i * 4) + 1] = randomFloats(gen) * 2.0f - 1.0f;
		randomRotations[(i * 4) + 2] = 0.0f;
		randomRotations[(i * 4) + 3] = 1.0f;
	}

	ImageTools::loadColorArray(randomRotations, &randomTexture.image, 4, 4, physicalDevice.colorFormat, &randomTexture.mipmapLevels, &randomTexture.allocationId);
	ImageTools::createImageView(&randomTexture.imageView, randomTexture.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&randomTexture.imageSampler, 1, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
}

void SSAO::draw(CommandBuffer* commandBuffer, uint32_t frameInFlightIndex) {
	// Depth to positions
	colorRenderPass.begin(commandBuffer, depthToPositionsFramebuffers[frameInFlightIndex].framebuffer, { static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height) });

	depthToPositionsGraphicsPipeline.bind(commandBuffer);
	depthToPositionsAndNormalsDescriptorSets[frameInFlightIndex].bind(commandBuffer, 0);

	vkCmdDraw(commandBuffer->commandBuffer, 3, 1, 0, 0);

	colorRenderPass.end(commandBuffer);

	// Depth to normals
	colorRenderPass.begin(commandBuffer, depthToNormalsFramebuffers[frameInFlightIndex].framebuffer, { static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height) });

	depthToNormalsGraphicsPipeline.bind(commandBuffer);
	depthToPositionsAndNormalsDescriptorSets[frameInFlightIndex].bind(commandBuffer, 0);

	vkCmdDraw(commandBuffer->commandBuffer, 3, 1, 0, 0);

	colorRenderPass.end(commandBuffer);

	// SSAO
	glm::vec2 imageSize = {viewport.viewport.width, viewport.viewport.height};

	colorRenderPass.begin(commandBuffer, ssaoFramebuffers[frameInFlightIndex].framebuffer, { static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height) });

	ssaoGraphicsPipeline.bind(commandBuffer);
	ssaoDescriptorSets[frameInFlightIndex].bind(commandBuffer, 0);
	ssaoGraphicsPipeline.pushConstant(commandBuffer, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 2 * sizeof(float), &imageSize);

	vkCmdDraw(commandBuffer->commandBuffer, 3, 1, 0, 0);

	colorRenderPass.end(commandBuffer);

	// SSAO blurred
	colorRenderPass.begin(commandBuffer, ssaoBlurredFramebuffers[frameInFlightIndex].framebuffer, { static_cast<uint32_t>(viewport.viewport.width), static_cast<uint32_t>(viewport.viewport.height) });

	ssaoBlurredGraphicsPipeline.bind(commandBuffer);
	ssaoBlurredDescriptorSets[frameInFlightIndex].bind(commandBuffer, 0);

	vkCmdDraw(commandBuffer->commandBuffer, 3, 1, 0, 0);

	colorRenderPass.end(commandBuffer);
}
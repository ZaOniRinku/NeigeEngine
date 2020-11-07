#include "Envmap.h"
#include "vulkan/vulkan.hpp"
#include "../../utils/resources/BufferTools.h"
#include "../../utils/resources/ImageTools.h"
#include "../../graphics/resources/RendererResources.h"

void Envmap::init(std::string filePath) {
	if (filePath != "") {
		ImageTools::loadHDREnvmap(filePath, &envmapImage.image, physicalDevice.colorFormat, &envmapImage.allocationId);
		ImageTools::createImageView(&envmapImage.imageView, envmapImage.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	ImageTools::createImage(&skyboxImage.image, 6, ENVMAP_WIDTH, ENVMAP_HEIGHT, 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &skyboxImage.allocationId);
	ImageTools::createImageView(&skyboxImage.imageView, skyboxImage.image, 0, 6, 0, 1, VK_IMAGE_VIEW_TYPE_CUBE, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&skyboxImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);

	float defaultSkyboxColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ImageTools::loadColorForEnvmap(defaultSkyboxColor, &defaultSkybox.image, physicalDevice.colorFormat, &defaultSkybox.mipmapLevels, &defaultSkybox.allocationId);
	ImageTools::createImageView(&defaultSkybox.imageView, defaultSkybox.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&defaultSkybox.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);

	std::vector<uint32_t> cubeIndices;
	cubeIndices.resize(cubeVertices.size());
	std::iota(cubeIndices.begin(), cubeIndices.end(), 0);

	std::vector<uint32_t> quadIndices;
	quadIndices.resize(quadVertices.size());
	std::iota(quadIndices.begin(), quadIndices.end(), 0);

	Buffer stagingVertexBuffer;
	VkDeviceSize size = cubeVertices.size() * sizeof(Vertex);
	BufferTools::createStagingBuffer(stagingVertexBuffer.buffer, stagingVertexBuffer.deviceMemory, size);
	void* cubeVertexData;
	stagingVertexBuffer.map(0, size, &cubeVertexData);
	memcpy(cubeVertexData, cubeVertices.data(), static_cast<size_t>(size));
	stagingVertexBuffer.unmap();
	BufferTools::createBuffer(cubeVertexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &cubeVertexBuffer.allocationId);
	BufferTools::copyBuffer(stagingVertexBuffer.buffer, cubeVertexBuffer.buffer, size);
	stagingVertexBuffer.destroy();

	Buffer stagingIndexBuffer;
	size = cubeIndices.size() * sizeof(uint32_t);
	BufferTools::createStagingBuffer(stagingIndexBuffer.buffer, stagingIndexBuffer.deviceMemory, size);
	void* cubeIndexData;
	stagingIndexBuffer.map(0, size, &cubeIndexData);
	memcpy(cubeIndexData, cubeIndices.data(), static_cast<size_t>(size));
	stagingIndexBuffer.unmap();
	BufferTools::createBuffer(cubeIndexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &cubeIndexBuffer.allocationId);
	BufferTools::copyBuffer(stagingIndexBuffer.buffer, cubeIndexBuffer.buffer, size);
	stagingIndexBuffer.destroy();
	
	size = quadVertices.size() * sizeof(Vertex);
	BufferTools::createStagingBuffer(stagingVertexBuffer.buffer, stagingVertexBuffer.deviceMemory, size);
	void* quadVertexData;
	stagingVertexBuffer.map(0, size, &quadVertexData);
	memcpy(quadVertexData, quadVertices.data(), static_cast<size_t>(size));
	stagingVertexBuffer.unmap();
	BufferTools::createBuffer(quadVertexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &quadVertexBuffer.allocationId);
	BufferTools::copyBuffer(stagingVertexBuffer.buffer, quadVertexBuffer.buffer, size);
	stagingVertexBuffer.destroy();

	size = quadIndices.size() * sizeof(uint32_t);
	BufferTools::createStagingBuffer(stagingIndexBuffer.buffer, stagingIndexBuffer.deviceMemory, size);
	void* quadIndexData;
	stagingIndexBuffer.map(0, size, &quadIndexData);
	memcpy(quadIndexData, quadIndices.data(), static_cast<size_t>(size));
	stagingIndexBuffer.unmap();
	BufferTools::createBuffer(quadIndexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &quadIndexBuffer.allocationId);
	BufferTools::copyBuffer(stagingIndexBuffer.buffer, quadIndexBuffer.buffer, size);
	stagingIndexBuffer.destroy();

	equilateralRectangleToCubemap();
	createDiffuseIradiance();
	createPrefilter();
	createBRDFConvolution();
}

void Envmap::destroy() {
	cubeVertexBuffer.destroy();
	cubeIndexBuffer.destroy();
	quadVertexBuffer.destroy();
	quadIndexBuffer.destroy();
	defaultSkybox.destroy();
	envmapImage.destroy();
	skyboxImage.destroy();
	diffuseIradianceImage.destroy();
	prefilterImage.destroy();
	brdfConvolutionImage.destroy();
}

void Envmap::draw(CommandBuffer* commandBuffer) {
	VkDeviceSize offset = 0;

	vkCmdBindVertexBuffers(commandBuffer->commandBuffer, 0, 1, &cubeVertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(commandBuffer->commandBuffer, cubeIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer->commandBuffer, 36, 1, 0, 0, 0);
}

void Envmap::equilateralRectangleToCubemap() {
	Viewport equiRecToCubemapViewport;
	equiRecToCubemapViewport.init(ENVMAP_WIDTH, ENVMAP_HEIGHT);

	std::vector<RenderPassAttachment> attachments;
	attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, physicalDevice.colorFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

	std::vector<SubpassDependency> dependencies;
	dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

	RenderPass equiRecToCubemapRenderPass;
	equiRecToCubemapRenderPass.init(attachments, dependencies);

	std::array<Framebuffer, 6> equiRecToCubemapFramebuffers;
	std::array<VkImageView, 6> equiRecToCubemapImageViews;
	for (int face = 0; face < 6; face++) {
		ImageTools::createImageView(&equiRecToCubemapImageViews[face], skyboxImage.image, face, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		std::vector<VkImageView> framebufferAttachments;
		framebufferAttachments.push_back(equiRecToCubemapImageViews[face]);
		equiRecToCubemapFramebuffers[face].init(&equiRecToCubemapRenderPass, framebufferAttachments, ENVMAP_WIDTH, ENVMAP_HEIGHT, 1);
	}

	GraphicsPipeline equiRecToCubemapGraphicsPipeline;
	equiRecToCubemapGraphicsPipeline.vertexShaderPath = "../shaders/equiRecToCubemap.vert";
	equiRecToCubemapGraphicsPipeline.fragmentShaderPath = "../shaders/equiRecToCubemap.frag";
	equiRecToCubemapGraphicsPipeline.renderPass = &equiRecToCubemapRenderPass;
	equiRecToCubemapGraphicsPipeline.viewport = &equiRecToCubemapViewport;
	equiRecToCubemapGraphicsPipeline.colorBlend = false;
	equiRecToCubemapGraphicsPipeline.multiSample = false;
	equiRecToCubemapGraphicsPipeline.init();
	
	DescriptorSet equiRecToCubemapDescriptorSet;
	equiRecToCubemapDescriptorSet.init(&equiRecToCubemapGraphicsPipeline, 0);

	VkDescriptorImageInfo skyboxInfo = {};
	skyboxInfo.sampler = defaultSkybox.imageSampler;
	skyboxInfo.imageView = envmapImage.image != VK_NULL_HANDLE ? envmapImage.imageView : defaultSkybox.imageView;
	skyboxInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	std::vector<VkWriteDescriptorSet> writesDescriptorSet;

	VkWriteDescriptorSet skyboxWriteDescriptorSet = {};
	skyboxWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	skyboxWriteDescriptorSet.pNext = nullptr;
	skyboxWriteDescriptorSet.dstSet = equiRecToCubemapDescriptorSet.descriptorSet;
	skyboxWriteDescriptorSet.dstBinding = 0;
	skyboxWriteDescriptorSet.dstArrayElement = 0;
	skyboxWriteDescriptorSet.descriptorCount = 1;
	skyboxWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	skyboxWriteDescriptorSet.pImageInfo = &skyboxInfo;
	skyboxWriteDescriptorSet.pBufferInfo = nullptr;
	skyboxWriteDescriptorSet.pTexelBufferView = nullptr;
	writesDescriptorSet.push_back(skyboxWriteDescriptorSet);

	equiRecToCubemapDescriptorSet.update(writesDescriptorSet);

	glm::mat4 projection = Camera::createPerspectiveProjection(90.0f, ENVMAP_WIDTH / static_cast<float>(ENVMAP_HEIGHT), 0.1f, 2.0f, true);
	std::array<glm::mat4, 6> views = { Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0)),
	};

	VkDeviceSize offset = 0;

	CommandPool commandPool;
	commandPool.init();
	CommandBuffer commandBuffer;
	commandBuffer.init(&commandPool);

	commandBuffer.begin();

	for (int face = 0; face < 6; face++) {
		glm::mat4 viewProj = projection * views[face];

		equiRecToCubemapRenderPass.begin(&commandBuffer, equiRecToCubemapFramebuffers[face].framebuffer, { ENVMAP_WIDTH, ENVMAP_HEIGHT });

		equiRecToCubemapGraphicsPipeline.bind(&commandBuffer);
		equiRecToCubemapGraphicsPipeline.pushConstant(&commandBuffer, VK_SHADER_STAGE_VERTEX_BIT, 0, 16 * sizeof(float), &viewProj);
		equiRecToCubemapDescriptorSet.bind(&commandBuffer, 0);

		vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, &cubeVertexBuffer.buffer, &offset);
		vkCmdBindIndexBuffer(commandBuffer.commandBuffer, cubeIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffer.commandBuffer, 36, 1, 0, 0, 0);

		equiRecToCubemapRenderPass.end(&commandBuffer);
	}
	
	commandBuffer.endAndSubmit();
	commandPool.destroy();

	equiRecToCubemapRenderPass.destroy();
	equiRecToCubemapGraphicsPipeline.destroy();
	for (int i = 0; i < 6; i++) {
		equiRecToCubemapFramebuffers[i].destroy();
		vkDestroyImageView(logicalDevice.device, equiRecToCubemapImageViews[i], nullptr);
	}
}

void Envmap::createDiffuseIradiance() {
	ImageTools::createImage(&diffuseIradianceImage.image, 6, CONVOLVE_WIDTH, CONVOLVE_HEIGHT, 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &diffuseIradianceImage.allocationId);
	ImageTools::createImageView(&diffuseIradianceImage.imageView, diffuseIradianceImage.image, 0, 6, 0, 1, VK_IMAGE_VIEW_TYPE_CUBE, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&diffuseIradianceImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);

	Viewport diffuseIradianceViewport;
	diffuseIradianceViewport.init(CONVOLVE_WIDTH, CONVOLVE_HEIGHT);

	std::vector<RenderPassAttachment> attachments;
	attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, physicalDevice.colorFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

	std::vector<SubpassDependency> dependencies;
	dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

	RenderPass convolveRenderPass;
	convolveRenderPass.init(attachments, dependencies);

	std::array<Framebuffer, 6> diffuseIradianceFramebuffers;
	std::array<VkImageView, 6> diffuseIradianceImageViews;
	for (int face = 0; face < 6; face++) {
		ImageTools::createImageView(&diffuseIradianceImageViews[face], diffuseIradianceImage.image, face, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		std::vector<VkImageView> framebufferAttachments;
		framebufferAttachments.push_back(diffuseIradianceImageViews[face]);
		diffuseIradianceFramebuffers[face].init(&convolveRenderPass, framebufferAttachments, CONVOLVE_WIDTH, CONVOLVE_HEIGHT, 1);
	}

	GraphicsPipeline convolveGraphicsPipeline;
	convolveGraphicsPipeline.vertexShaderPath = "../shaders/projectToCube.vert";
	convolveGraphicsPipeline.fragmentShaderPath = "../shaders/convolve.frag";
	convolveGraphicsPipeline.renderPass = &convolveRenderPass;
	convolveGraphicsPipeline.viewport = &diffuseIradianceViewport;
	convolveGraphicsPipeline.colorBlend = false;
	convolveGraphicsPipeline.multiSample = false;
	convolveGraphicsPipeline.init();

	DescriptorSet convolveDescriptorSet;
	convolveDescriptorSet.init(&convolveGraphicsPipeline, 0);

	VkDescriptorImageInfo skyboxInfo = {};
	skyboxInfo.sampler = defaultSkybox.imageSampler;
	skyboxInfo.imageView = skyboxImage.image != VK_NULL_HANDLE ? skyboxImage.imageView : defaultSkybox.imageView;
	skyboxInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	std::vector<VkWriteDescriptorSet> writesDescriptorSet;

	VkWriteDescriptorSet skyboxWriteDescriptorSet = {};
	skyboxWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	skyboxWriteDescriptorSet.pNext = nullptr;
	skyboxWriteDescriptorSet.dstSet = convolveDescriptorSet.descriptorSet;
	skyboxWriteDescriptorSet.dstBinding = 0;
	skyboxWriteDescriptorSet.dstArrayElement = 0;
	skyboxWriteDescriptorSet.descriptorCount = 1;
	skyboxWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	skyboxWriteDescriptorSet.pImageInfo = &skyboxInfo;
	skyboxWriteDescriptorSet.pBufferInfo = nullptr;
	skyboxWriteDescriptorSet.pTexelBufferView = nullptr;
	writesDescriptorSet.push_back(skyboxWriteDescriptorSet);

	convolveDescriptorSet.update(writesDescriptorSet);

	glm::mat4 projection = Camera::createPerspectiveProjection(90.0f, CONVOLVE_WIDTH / static_cast<float>(CONVOLVE_HEIGHT), 0.1f, 2.0f, true);
	std::array<glm::mat4, 6> views = { Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0)),
		Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0)),
	};

	VkDeviceSize offset = 0;

	CommandPool commandPool;
	commandPool.init();
	CommandBuffer commandBuffer;
	commandBuffer.init(&commandPool);

	commandBuffer.begin();

	for (int face = 0; face < 6; face++) {
		glm::mat4 viewProj = projection * views[face];

		convolveRenderPass.begin(&commandBuffer, diffuseIradianceFramebuffers[face].framebuffer, { CONVOLVE_WIDTH, CONVOLVE_HEIGHT });

		convolveGraphicsPipeline.bind(&commandBuffer);
		convolveGraphicsPipeline.pushConstant(&commandBuffer, VK_SHADER_STAGE_VERTEX_BIT, 0, 16 * sizeof(float), &viewProj);
		convolveDescriptorSet.bind(&commandBuffer, 0);

		vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, &cubeVertexBuffer.buffer, &offset);
		vkCmdBindIndexBuffer(commandBuffer.commandBuffer, cubeIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffer.commandBuffer, 36, 1, 0, 0, 0);

		convolveRenderPass.end(&commandBuffer);
	}

	commandBuffer.endAndSubmit();
	commandPool.destroy();

	convolveRenderPass.destroy();
	convolveGraphicsPipeline.destroy();
	for (int i = 0; i < 6; i++) {
		diffuseIradianceFramebuffers[i].destroy();
		vkDestroyImageView(logicalDevice.device, diffuseIradianceImageViews[i], nullptr);
	}
}

void Envmap::createPrefilter() {
	ImageTools::createImage(&prefilterImage.image, 6, PREFILTER_WIDTH, PREFILTER_HEIGHT, 5, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prefilterImage.allocationId);
	ImageTools::createImageView(&prefilterImage.imageView, prefilterImage.image, 0, 6, 0, 5, VK_IMAGE_VIEW_TYPE_CUBE, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&prefilterImage.imageSampler, 5, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);

	std::vector<RenderPassAttachment> attachments;
	attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, physicalDevice.colorFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

	std::vector<SubpassDependency> dependencies;
	dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

	RenderPass prefilterRenderPass;
	prefilterRenderPass.init(attachments, dependencies);

	GraphicsPipeline prefilterGraphicsPipeline;
	prefilterGraphicsPipeline.vertexShaderPath = "../shaders/projectToCube.vert";
	prefilterGraphicsPipeline.fragmentShaderPath = "../shaders/prefilter.frag";
	prefilterGraphicsPipeline.renderPass = &prefilterRenderPass;
	prefilterGraphicsPipeline.colorBlend = false;
	prefilterGraphicsPipeline.multiSample = false;

	std::array<Framebuffer, 30> prefilterFramebuffers;
	std::array<VkImageView, 30> prefilterImageViews;

	Buffer roughnessBuffer;
	BufferTools::createUniformBuffer(roughnessBuffer.buffer, roughnessBuffer.deviceMemory, sizeof(float));

	for (int mipLevel = 0; mipLevel < 5; mipLevel++) {
		uint32_t mipWidth = static_cast<uint32_t>(PREFILTER_WIDTH * std::pow(0.5f, mipLevel));
		uint32_t mipHeight = static_cast<uint32_t>(PREFILTER_HEIGHT * std::pow(0.5f, mipLevel));

		Viewport prefilterViewport;
		prefilterViewport.init(mipWidth, mipHeight);

		for (int face = 0; face < 6; face++) {
			ImageTools::createImageView(&prefilterImageViews[mipLevel * 6 + face], prefilterImage.image, face, 1, mipLevel, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
			std::vector<VkImageView> framebufferAttachments;
			framebufferAttachments.push_back(prefilterImageViews[mipLevel * 6 + face]);
			prefilterFramebuffers[mipLevel * 6 + face].init(&prefilterRenderPass, framebufferAttachments, mipWidth, mipHeight, 1);
		}

		void* data;

		float roughness = static_cast<float>(mipLevel) / 4.0f;

		roughnessBuffer.map(0, sizeof(float), &data);
		memcpy(data, &roughness, sizeof(float));
		roughnessBuffer.unmap();

		prefilterGraphicsPipeline.viewport = &prefilterViewport;
		prefilterGraphicsPipeline.init();

		DescriptorSet prefilterDescriptorSet;
		prefilterDescriptorSet.init(&prefilterGraphicsPipeline, 0);

		VkDescriptorImageInfo skyboxInfo = {};
		skyboxInfo.sampler = defaultSkybox.imageSampler;
		skyboxInfo.imageView = skyboxImage.image != VK_NULL_HANDLE ? skyboxImage.imageView : defaultSkybox.imageView;
		skyboxInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorBufferInfo roughnessInfo = {};
		roughnessInfo.buffer = roughnessBuffer.buffer;
		roughnessInfo.offset = 0;
		roughnessInfo.range = sizeof(float);

		std::vector<VkWriteDescriptorSet> writesDescriptorSet;

		VkWriteDescriptorSet skyboxWriteDescriptorSet = {};
		skyboxWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		skyboxWriteDescriptorSet.pNext = nullptr;
		skyboxWriteDescriptorSet.dstSet = prefilterDescriptorSet.descriptorSet;
		skyboxWriteDescriptorSet.dstBinding = 0;
		skyboxWriteDescriptorSet.dstArrayElement = 0;
		skyboxWriteDescriptorSet.descriptorCount = 1;
		skyboxWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		skyboxWriteDescriptorSet.pImageInfo = &skyboxInfo;
		skyboxWriteDescriptorSet.pBufferInfo = nullptr;
		skyboxWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(skyboxWriteDescriptorSet);

		VkWriteDescriptorSet roughnessWriteDescriptorSet = {};
		roughnessWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		roughnessWriteDescriptorSet.pNext = nullptr;
		roughnessWriteDescriptorSet.dstSet = prefilterDescriptorSet.descriptorSet;
		roughnessWriteDescriptorSet.dstBinding = 1;
		roughnessWriteDescriptorSet.dstArrayElement = 0;
		roughnessWriteDescriptorSet.descriptorCount = 1;
		roughnessWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		roughnessWriteDescriptorSet.pImageInfo = nullptr;
		roughnessWriteDescriptorSet.pBufferInfo = &roughnessInfo;
		roughnessWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(roughnessWriteDescriptorSet);

		prefilterDescriptorSet.update(writesDescriptorSet);

		glm::mat4 projection = Camera::createPerspectiveProjection(90.0f, mipWidth / static_cast<float>(mipHeight), 0.1f, 2.0f, true);
		std::array<glm::mat4, 6> views = { Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0)),
			Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0)),
			Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0)),
			Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0)),
			Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0)),
			Camera::createLookAtView(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0)),
		};

		VkDeviceSize offset = 0;

		CommandPool commandPool;
		commandPool.init();
		CommandBuffer commandBuffer;
		commandBuffer.init(&commandPool);

		commandBuffer.begin();

		for (int face = 0; face < 6; face++) {
			glm::mat4 viewProj = projection * views[face];

			prefilterRenderPass.begin(&commandBuffer, prefilterFramebuffers[mipLevel * 6 + face].framebuffer, { mipWidth, mipHeight });

			prefilterGraphicsPipeline.bind(&commandBuffer);
			prefilterGraphicsPipeline.pushConstant(&commandBuffer, VK_SHADER_STAGE_VERTEX_BIT, 0, 16 * sizeof(float), &viewProj);
			prefilterDescriptorSet.bind(&commandBuffer, 0);

			vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, &cubeVertexBuffer.buffer, &offset);
			vkCmdBindIndexBuffer(commandBuffer.commandBuffer, cubeIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(commandBuffer.commandBuffer, 36, 1, 0, 0, 0);

			prefilterRenderPass.end(&commandBuffer);
		}

		commandBuffer.endAndSubmit();
		commandPool.destroy();

		prefilterGraphicsPipeline.destroyPipeline();
	}

	prefilterRenderPass.destroy();
	prefilterGraphicsPipeline.destroy();
	for (int i = 0; i < 30; i++) {
		prefilterFramebuffers[i].destroy();
		vkDestroyImageView(logicalDevice.device, prefilterImageViews[i], nullptr);
	}
	roughnessBuffer.destroy();
}

void Envmap::createBRDFConvolution() {
	ImageTools::createImage(&brdfConvolutionImage.image, 1, BRDFCONVOLUTION_WIDTH, BRDFCONVOLUTION_HEIGHT, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &brdfConvolutionImage.allocationId);
	ImageTools::createImageView(&brdfConvolutionImage.imageView, brdfConvolutionImage.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&brdfConvolutionImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);

	Viewport brdfConvolutionViewport;
	brdfConvolutionViewport.init(BRDFCONVOLUTION_WIDTH, BRDFCONVOLUTION_HEIGHT);

	std::vector<RenderPassAttachment> attachments;
	attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, VK_FORMAT_R32G32_SFLOAT, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

	std::vector<SubpassDependency> dependencies;
	dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

	RenderPass brdfConvolutionRenderPass;
	brdfConvolutionRenderPass.init(attachments, dependencies);

	Framebuffer brdfConvolutionFramebuffer;
	std::vector<VkImageView> framebufferAttachments;
	framebufferAttachments.push_back(brdfConvolutionImage.imageView);
	brdfConvolutionFramebuffer.init(&brdfConvolutionRenderPass, framebufferAttachments, BRDFCONVOLUTION_WIDTH, BRDFCONVOLUTION_HEIGHT, 1);

	GraphicsPipeline brdfConvolutionGraphicsPipeline;
	brdfConvolutionGraphicsPipeline.vertexShaderPath = "../shaders/brdfConvolution.vert";
	brdfConvolutionGraphicsPipeline.fragmentShaderPath = "../shaders/brdfConvolution.frag";
	brdfConvolutionGraphicsPipeline.renderPass = &brdfConvolutionRenderPass;
	brdfConvolutionGraphicsPipeline.viewport = &brdfConvolutionViewport;
	brdfConvolutionGraphicsPipeline.colorBlend = false;
	brdfConvolutionGraphicsPipeline.multiSample = false;
	brdfConvolutionGraphicsPipeline.init();

	VkDeviceSize offset = 0;

	CommandPool commandPool;
	commandPool.init();
	CommandBuffer commandBuffer;
	commandBuffer.init(&commandPool);

	commandBuffer.begin();

	brdfConvolutionRenderPass.begin(&commandBuffer, brdfConvolutionFramebuffer.framebuffer, { BRDFCONVOLUTION_WIDTH, BRDFCONVOLUTION_HEIGHT });

	brdfConvolutionGraphicsPipeline.bind(&commandBuffer);

	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, &quadVertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(commandBuffer.commandBuffer, quadIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer.commandBuffer, 6, 1, 0, 0, 0);

	brdfConvolutionRenderPass.end(&commandBuffer);

	commandBuffer.endAndSubmit();
	commandPool.destroy();

	brdfConvolutionRenderPass.destroy();
	brdfConvolutionGraphicsPipeline.destroy();
	brdfConvolutionFramebuffer.destroy();
}

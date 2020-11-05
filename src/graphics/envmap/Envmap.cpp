#include "Envmap.h"
#include "vulkan/vulkan.hpp"
#include "../../utils/resources/BufferTools.h"
#include "../../utils/resources/ImageTools.h"
#include "../../graphics/resources/RendererResources.h"

void Envmap::init(std::string filePath) {
	if (filePath != "") {
		ImageTools::loadHDREnvmap(filePath, &envmapImage.image, physicalDevice.colorFormat, &envmapImage.allocationId);
		ImageTools::createImageView(&envmapImage.imageView, envmapImage.image, 0, 1, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	ImageTools::createImage(&skyboxImage.image, 6, ENVMAP_WIDTH, ENVMAP_HEIGHT, 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &skyboxImage.allocationId);
	ImageTools::createImageView(&skyboxImage.imageView, skyboxImage.image, 0, 6, 1, VK_IMAGE_VIEW_TYPE_CUBE, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&skyboxImage.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);

	float defaultSkyboxColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	ImageTools::loadColorForEnvmap(defaultSkyboxColor, &defaultSkybox.image, physicalDevice.colorFormat, &defaultSkybox.mipmapLevels, &defaultSkybox.allocationId);
	ImageTools::createImageView(&defaultSkybox.imageView, defaultSkybox.image, 0, 1, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&defaultSkybox.imageSampler, 1, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);

	std::vector<uint32_t> cubeIndices;
	cubeIndices.resize(cubeVertices.size());
	std::iota(cubeIndices.begin(), cubeIndices.end(), 0);

	Buffer stagingVertexBuffer;
	VkDeviceSize size = cubeVertices.size() * sizeof(Vertex);
	BufferTools::createStagingBuffer(stagingVertexBuffer.buffer, stagingVertexBuffer.deviceMemory, size);
	void* vertexData;
	stagingVertexBuffer.map(0, size, &vertexData);
	memcpy(vertexData, cubeVertices.data(), static_cast<size_t>(size));
	stagingVertexBuffer.unmap();
	BufferTools::createBuffer(cubeVertexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &cubeVertexBuffer.allocationId);
	BufferTools::copyBuffer(stagingVertexBuffer.buffer, cubeVertexBuffer.buffer, size);
	stagingVertexBuffer.destroy();

	Buffer stagingIndexBuffer;
	size = cubeIndices.size() * sizeof(uint32_t);
	BufferTools::createStagingBuffer(stagingIndexBuffer.buffer, stagingIndexBuffer.deviceMemory, size);
	void* indexData;
	stagingIndexBuffer.map(0, size, &indexData);
	memcpy(indexData, cubeIndices.data(), static_cast<size_t>(size));
	stagingIndexBuffer.unmap();
	BufferTools::createBuffer(cubeIndexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &cubeIndexBuffer.allocationId);
	BufferTools::copyBuffer(stagingIndexBuffer.buffer, cubeIndexBuffer.buffer, size);
	stagingIndexBuffer.destroy();

	equilateralRectangleToCubemap();
}

void Envmap::destroy() {
	equiRecToCubemapRenderPass.destroy();
	equiRecToCubemapGraphicsPipeline.destroy();
	for (Framebuffer& framebuffer : equiRecToCubemapFramebuffers) {
		framebuffer.destroy();
	}
	cubeVertexBuffer.destroy();
	cubeIndexBuffer.destroy();
	defaultSkybox.destroy();
	envmapImage.destroy();
	skyboxImage.destroy();
	for (int i = 0; i < 6; i++) {
		vkDestroyImageView(logicalDevice.device, equiRecToCubemapImageViews[i], nullptr);
	}
	skyboxGraphicsPipeline.destroy();
}

void Envmap::draw(CommandBuffer* commandBuffer) {
	VkDeviceSize offset = 0;

	vkCmdBindVertexBuffers(commandBuffer->commandBuffer, 0, 1, &cubeVertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(commandBuffer->commandBuffer, cubeIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer->commandBuffer, 36, 1, 0, 0, 0);
}

void Envmap::equilateralRectangleToCubemap() {
	equiRecToCubemapViewport.init(ENVMAP_WIDTH, ENVMAP_HEIGHT);

	std::vector<RenderPassAttachment> attachments;
	attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, physicalDevice.colorFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

	std::vector<SubpassDependency> dependencies;
	dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

	equiRecToCubemapRenderPass.init(attachments, dependencies);

	for (int i = 0; i < 6; i++) {
		ImageTools::createImageView(&equiRecToCubemapImageViews[i], skyboxImage.image, i, 1, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		std::vector<VkImageView> framebufferAttachments;
		framebufferAttachments.push_back(equiRecToCubemapImageViews[i]);
		equiRecToCubemapFramebuffers[i].init(&equiRecToCubemapRenderPass, framebufferAttachments, ENVMAP_WIDTH, ENVMAP_HEIGHT, 1);
	}

	equiRecToCubemapGraphicsPipeline.vertexShaderPath = "../shaders/equiRecToCubemap.vert";
	equiRecToCubemapGraphicsPipeline.fragmentShaderPath = "../shaders/equiRecToCubemap.frag";
	equiRecToCubemapGraphicsPipeline.renderPass = &equiRecToCubemapRenderPass;
	equiRecToCubemapGraphicsPipeline.viewport = &equiRecToCubemapViewport;
	equiRecToCubemapGraphicsPipeline.colorBlend = false;
	equiRecToCubemapGraphicsPipeline.multiSample = false;
	equiRecToCubemapGraphicsPipeline.init();
	
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

	for (int i = 0; i < 6; i++) {
		glm::mat4 viewProj = projection * views[i];

		equiRecToCubemapRenderPass.begin(&commandBuffer, equiRecToCubemapFramebuffers[i].framebuffer, { ENVMAP_WIDTH, ENVMAP_HEIGHT });

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
}
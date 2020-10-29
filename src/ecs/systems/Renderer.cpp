#include "Renderer.h"
#include "../../graphics/resources/RendererResources.h"
#include "../../inputs/Inputs.h"
#include "../components/Transform.h"
#include "../components/Camera.h"
#include "../components/Renderable.h"

extern ECS ecs;

void Renderer::init() {
	// Instance
	instance.init(VK_MAKE_VERSION(0, 0, 1), window->instanceExtensions());

	// Surface
	window->createSurface();

	// Pick physical device
	PhysicalDevicePicker::pick(window);

	// Logical device
	logicalDevice.init();

	// Swapchain
	swapchain.init(window, &swapchainSize);

	NEIGE_INFO("Max frames in flight : " + std::to_string(MAX_FRAMES_IN_FLIGHT));
	NEIGE_INFO("Swapchain size : " + std::to_string(swapchainSize));
	NEIGE_INFO("Swapchain format : " + NeigeVKTranslate::vkFormatToString(swapchain.surfaceFormat.format));
	NEIGE_INFO("Swapchain color space : " + NeigeVKTranslate::vkColorSpaceToString(swapchain.surfaceFormat.colorSpace));
	NEIGE_INFO("Present mode : " + NeigeVKTranslate::vkPresentModeToString(swapchain.presentMode));

	// Camera
	camera = ecs.createEntity();
	ecs.addComponent(camera, Transform{
		glm::vec3(0.0f),
		glm::vec3(0.0f),
		glm::vec3(0.0f)
		});
	ecs.addComponent(camera, Camera{
		glm::vec3(1.0f, 0.0f, 0.0f),
		Camera::createPerspectiveProjection(45.0f, window->extent.width / static_cast<float>(window->extent.height), 0.1f, 1000.0f)
		});
	cameraBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (Buffer& buffer : cameraBuffers) {
		BufferTools::createUniformBuffer(buffer.buffer, buffer.deviceMemory, sizeof(CameraUniformBufferObject));
	}

	// Render passes
	std::vector<RenderPassAttachment> attachments;
	attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, swapchain.surfaceFormat.format, physicalDevice.maxUsableSampleCount, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE));
	attachments.push_back(RenderPassAttachment(AttachmentType::DEPTH, physicalDevice.depthFormat, physicalDevice.maxUsableSampleCount, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE));
	attachments.push_back(RenderPassAttachment(AttachmentType::SWAPCHAIN, swapchain.surfaceFormat.format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE));

	std::vector<SubpassDependency> dependencies;
	dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0 });
	RenderPass renderPass;
	renderPass.init(attachments, dependencies);
	renderPasses.push_back(renderPass);

	// Framebuffers
	Image colorAttachment;
	colorAttachment.allocationId = ImageTools::createImage(&colorAttachment.image, 1, window->extent.width, window->extent.height, 1, physicalDevice.maxUsableSampleCount, swapchain.surfaceFormat.format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	ImageTools::createImageView(&colorAttachment.imageView, colorAttachment.image, 1, 1, VK_IMAGE_VIEW_TYPE_2D, swapchain.surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::transitionLayout(colorAttachment.image, swapchain.surfaceFormat.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);
	colorImages.push_back(colorAttachment);
	Image depthAttachment;
	depthAttachment.allocationId = ImageTools::createImage(&depthAttachment.image, 1, window->extent.width, window->extent.height, 1, physicalDevice.maxUsableSampleCount, physicalDevice.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	ImageTools::createImageView(&depthAttachment.imageView, depthAttachment.image, 1, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	ImageTools::transitionLayout(depthAttachment.image, physicalDevice.depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);
	depthImages.push_back(depthAttachment);
	std::vector<std::vector<VkImageView>> framebufferAttachments;
	framebufferAttachments.resize(swapchainSize);
	framebuffers.resize(swapchainSize);
	for (size_t i = 0; i < framebufferAttachments.size(); i++) {
		framebufferAttachments[i].push_back(colorAttachment.imageView);
		framebufferAttachments[i].push_back(depthAttachment.imageView);
		framebufferAttachments[i].push_back(swapchain.imageViews[i]);
		framebuffers[i].init(&renderPasses[0], framebufferAttachments[i], window->extent.width, window->extent.height);
	}

	// Fullscreen viewport
	fullscreenViewport.init(window->extent.width, window->extent.height);

	for (Entity entity : entities) {
		auto const& renderable = ecs.getComponent<Renderable>(entity);
		std::string mapKey = renderable.vertexShaderPath + renderable.fragmentShaderPath + renderable.tesselationControlShaderPath + renderable.tesselationEvaluationShaderPath + renderable.geometryShaderPath;

		// Graphics pipelines
		if (graphicsPipelines.find(mapKey) == graphicsPipelines.end()) {
			GraphicsPipeline graphicsPipeline;
			graphicsPipeline.vertexShaderPath = renderable.vertexShaderPath;
			graphicsPipeline.fragmentShaderPath = renderable.fragmentShaderPath;
			graphicsPipeline.tesselationControlShaderPath = renderable.tesselationControlShaderPath;
			graphicsPipeline.tesselationEvaluationShaderPath = renderable.tesselationEvaluationShaderPath;
			graphicsPipeline.geometryShaderPath = renderable.geometryShaderPath;
			graphicsPipeline.init(true, &renderPasses[0], &fullscreenViewport);
			graphicsPipelines.emplace(mapKey, graphicsPipeline);
		}

		if (graphicsPipelines.at(mapKey).layoutBindings.size() != 0) {
			std::vector<Buffer> buffers;
			buffers.resize(MAX_FRAMES_IN_FLIGHT);

			std::vector<DescriptorSet> descriptorSets;
			descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				// Buffers
				BufferTools::createUniformBuffer(buffers[i].buffer, buffers[i].deviceMemory, sizeof(ObjectUniformBufferObject));

				// Descriptor sets
				descriptorSets[i].init(&graphicsPipelines.at(mapKey));

				VkDescriptorBufferInfo objectInfo = {};
				objectInfo.buffer = buffers.at(i).buffer;
				objectInfo.offset = 0;
				objectInfo.range = sizeof(ObjectUniformBufferObject);

				VkDescriptorBufferInfo cameraInfo = {};
				cameraInfo.buffer = cameraBuffers.at(i).buffer;
				cameraInfo.offset = 0;
				cameraInfo.range = sizeof(CameraUniformBufferObject);

				std::vector<VkWriteDescriptorSet> writesDescriptorSet;

				VkWriteDescriptorSet objectWriteDescriptorSet = {};
				objectWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				objectWriteDescriptorSet.pNext = nullptr;
				objectWriteDescriptorSet.dstSet = descriptorSets[i].descriptorSet;
				objectWriteDescriptorSet.dstBinding = 0;
				objectWriteDescriptorSet.dstArrayElement = 0;
				objectWriteDescriptorSet.descriptorCount = 1;
				objectWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				objectWriteDescriptorSet.pImageInfo = nullptr;
				objectWriteDescriptorSet.pBufferInfo = &objectInfo;
				objectWriteDescriptorSet.pTexelBufferView = nullptr;
				writesDescriptorSet.push_back(objectWriteDescriptorSet);

				VkWriteDescriptorSet cameraWriteDescriptorSet = {};
				cameraWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				cameraWriteDescriptorSet.pNext = nullptr;
				cameraWriteDescriptorSet.dstSet = descriptorSets[i].descriptorSet;
				cameraWriteDescriptorSet.dstBinding = 1;
				cameraWriteDescriptorSet.dstArrayElement = 0;
				cameraWriteDescriptorSet.descriptorCount = 1;
				cameraWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				cameraWriteDescriptorSet.pImageInfo = nullptr;
				cameraWriteDescriptorSet.pBufferInfo = &cameraInfo;
				cameraWriteDescriptorSet.pTexelBufferView = nullptr;
				writesDescriptorSet.push_back(cameraWriteDescriptorSet);

				descriptorSets[i].update(writesDescriptorSet);
			}

			entityBuffers.emplace(entity, buffers);
			entityDescriptorSets.emplace(entity, descriptorSets);
		}
	}

	// Command pools and buffers
	renderingCommandPools.resize(MAX_FRAMES_IN_FLIGHT);
	renderingCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		renderingCommandPools[i].init();
		renderingCommandBuffers[i].init(&renderingCommandPools[i]);
	}

	// Sync objects
	fences.resize(MAX_FRAMES_IN_FLIGHT);
	IAsemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	RFsemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		fences[i].init();
		IAsemaphores[i].init();
		RFsemaphores[i].init();
	}

	// Vertices and indices
	std::vector<Vertex> vertices;
	vertices.push_back({ glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) });
	vertices.push_back({ glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) });
	vertices.push_back({ glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) });
	vertices.push_back({ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) });
	std::vector<uint32_t> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);

	Buffer stagingVertexBuffer;
	vertexBuffer.size = vertices.size() * sizeof(Vertex);
	BufferTools::createStagingBuffer(stagingVertexBuffer.buffer, stagingVertexBuffer.deviceMemory, vertexBuffer.size);
	void* vertexData;
	stagingVertexBuffer.map(0, vertexBuffer.size, &vertexData);
	memcpy(vertexData, vertices.data(), static_cast<size_t>(vertexBuffer.size));
	stagingVertexBuffer.unmap();
	vertexBuffer.allocationId = BufferTools::createBuffer(vertexBuffer.buffer, vertexBuffer.size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	BufferTools::copyBuffer(stagingVertexBuffer.buffer, vertexBuffer.buffer, vertexBuffer.size);
	stagingVertexBuffer.destroy();

	Buffer stagingIndexBuffer;
	indexBuffer.size = indices.size() * sizeof(uint32_t);
	BufferTools::createStagingBuffer(stagingIndexBuffer.buffer, stagingIndexBuffer.deviceMemory, indexBuffer.size);
	void* indexData;
	stagingIndexBuffer.map(0, indexBuffer.size, &indexData);
	memcpy(indexData, indices.data(), static_cast<size_t>(indexBuffer.size));
	stagingIndexBuffer.unmap();
	indexBuffer.allocationId = BufferTools::createBuffer(indexBuffer.buffer, indexBuffer.size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	BufferTools::copyBuffer(stagingIndexBuffer.buffer, indexBuffer.buffer, indexBuffer.size);
	stagingIndexBuffer.destroy();
}

void Renderer::update() {
	if (NEIGE_DEBUG) {
		if (keyboardInputs.pKey == KeyState::PRESSED) {
			logicalDevice.wait();
			for (std::unordered_map<std::string, Shader>::iterator it = shaders.begin(); it != shaders.end(); it++) {
				it->second.reload();
			}
			for (std::unordered_map<std::string, GraphicsPipeline>::iterator it = graphicsPipelines.begin(); it != graphicsPipelines.end(); it++) {
				GraphicsPipeline* graphicsPipeline = &it->second;
				graphicsPipeline->destroyPipeline();
				graphicsPipeline->init(true, &renderPasses[0], &fullscreenViewport);
			}
		}
	}

	if (window->gotResized) {
		window->gotResized = false;
		while (window->extent.width == 0 || window->extent.height == 0) {
			window->waitEvents();
		}
		logicalDevice.wait();
		destroyResources();
		createResources();
		fullscreenViewport.viewport.width = static_cast<float>(window->extent.width);
		fullscreenViewport.viewport.height = static_cast<float>(window->extent.height);
		fullscreenViewport.scissor.extent.width = window->extent.width;
		fullscreenViewport.scissor.extent.height = window->extent.height;
		auto& cameraCamera = ecs.getComponent<Camera>(camera);
		cameraCamera.projection = Camera::createPerspectiveProjection(45.0f, window->extent.width / static_cast<float>(window->extent.height), 0.1f, 1000.0f);
	}

	fences[currentFrame].wait();

	uint32_t swapchainImage;
	VkResult result = swapchain.acquireNextImage(&IAsemaphores[currentFrame], &swapchainImage);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		while (window->extent.width == 0 || window->extent.height == 0) {
			window->waitEvents();
		}
		logicalDevice.wait();
		destroyResources();
		createResources();
		fullscreenViewport.viewport.width = static_cast<float>(window->extent.width);
		fullscreenViewport.viewport.height = static_cast<float>(window->extent.height);
		fullscreenViewport.scissor.extent.width = window->extent.width;
		fullscreenViewport.scissor.extent.height = window->extent.height;
		auto& cameraCamera = ecs.getComponent<Camera>(camera);
		cameraCamera.projection = Camera::createPerspectiveProjection(45.0f, window->extent.width / static_cast<float>(window->extent.height), 0.1f, 1000.0f);
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		NEIGE_ERROR("Unable to acquire swapchain image.");
	}

	updateData(currentFrame);

	recordRenderingCommands(currentFrame, swapchainImage);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &IAsemaphores[currentFrame].semaphore;
	VkPipelineStageFlags stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = stages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &renderingCommandBuffers[currentFrame].commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &RFsemaphores[currentFrame].semaphore;

	fences[currentFrame].reset();
	NEIGE_VK_CHECK(vkQueueSubmit(logicalDevice.queues.graphicsQueue, 1, &submitInfo, fences[currentFrame].fence));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &RFsemaphores[currentFrame].semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.swapchain;
	presentInfo.pImageIndices = &swapchainImage;
	presentInfo.pResults = nullptr;
	result = vkQueuePresentKHR(logicalDevice.queues.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->gotResized) {
		window->gotResized = false;
		while (window->extent.width == 0 || window->extent.height == 0) {
			window->waitEvents();
		}
		logicalDevice.wait();
		destroyResources();
		createResources();
		fullscreenViewport.viewport.width = static_cast<float>(window->extent.width);
		fullscreenViewport.viewport.height = static_cast<float>(window->extent.height);
		fullscreenViewport.scissor.extent.width = window->extent.width;
		fullscreenViewport.scissor.extent.height = window->extent.height;
		auto& cameraCamera = ecs.getComponent<Camera>(camera);
		cameraCamera.projection = Camera::createPerspectiveProjection(45.0f, window->extent.width / static_cast<float>(window->extent.height), 0.1f, 1000.0f);
	}
	else if (result != VK_SUCCESS) {
		NEIGE_ERROR("Unable to present image to the swapchain.");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::destroy() {
	logicalDevice.wait();
	destroyResources();
	for (CommandPool& renderingCommandPool : renderingCommandPools) {
		renderingCommandPool.destroy();
	}
	for (RenderPass& renderPass : renderPasses) {
		renderPass.destroy();
	}
	for (Buffer& buffer : cameraBuffers) {
		buffer.destroy();
	}
	for (std::unordered_map<Entity, std::vector<Buffer>>::iterator it = entityBuffers.begin(); it != entityBuffers.end(); it++) {
		for (Buffer& buffer : it->second) {
			buffer.destroy();
		}
	}
	for (std::unordered_map<std::string, GraphicsPipeline>::iterator it = graphicsPipelines.begin(); it != graphicsPipelines.end(); it++) {
		GraphicsPipeline* graphicsPipeline = &it->second;
		graphicsPipeline->destroy();
	}
	for (std::unordered_map<std::string, Shader>::iterator it = shaders.begin(); it != shaders.end(); it++) {
		Shader* shader = &it->second;
		shader->destroy();
	}
	for (Fence& fence : fences) {
		fence.destroy();
	}
	for (Semaphore& IAsemaphore : IAsemaphores) {
		IAsemaphore.destroy();
	}
	for (Semaphore& RFsemaphore : RFsemaphores) {
		RFsemaphore.destroy();
	}
	vertexBuffer.destroy();
	indexBuffer.destroy();
	memoryAllocator.destroy();
	window->surface.destroy();
	logicalDevice.destroy();
	instance.destroy();
}

void Renderer::updateData(uint32_t frameInFlightIndex) {
	void* data;

	const auto& cameraTransform = ecs.getComponent<Transform>(camera);
	const auto& cameraCamera = ecs.getComponent<Camera>(camera);

	CameraUniformBufferObject cubo = {};
	glm::mat4 view = Camera::createLookAtView(cameraTransform.position, cameraTransform.position + cameraCamera.to, glm::vec3(0.0f, 1.0f, 0.0f));
	cubo.viewProj = cameraCamera.projection * view;
	cubo.position = cameraTransform.position;

	cameraBuffers.at(frameInFlightIndex).map(0, sizeof(CameraUniformBufferObject), &data);
	memcpy(data, &cubo, sizeof(CameraUniformBufferObject));
	cameraBuffers.at(frameInFlightIndex).unmap();

	for (Entity entity : entities) {
		const auto& objectTransform = ecs.getComponent<Transform>(entity);
		auto const& renderable = ecs.getComponent<Renderable>(entity);
		std::string mapKey = renderable.vertexShaderPath + renderable.fragmentShaderPath + renderable.tesselationControlShaderPath + renderable.tesselationEvaluationShaderPath + renderable.geometryShaderPath;

		if (graphicsPipelines.at(mapKey).layoutBindings.size() != 0) {
			ObjectUniformBufferObject oubo = {};
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), objectTransform.position);
			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(objectTransform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(objectTransform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(objectTransform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), objectTransform.scale);
			oubo.model = translate * rotateX * rotateY * rotateZ * scale;

			entityBuffers.at(entity).at(frameInFlightIndex).map(0, sizeof(ObjectUniformBufferObject), &data);
			memcpy(data, &oubo, sizeof(ObjectUniformBufferObject));
			entityBuffers.at(entity).at(frameInFlightIndex).unmap();
		}
	}
}

void Renderer::recordRenderingCommands(uint32_t frameInFlightIndex, uint32_t framebufferIndex) {
	renderingCommandPools[frameInFlightIndex].reset();
	renderingCommandBuffers[frameInFlightIndex].begin();

	VkDeviceSize offset = 0;
	renderPasses[0].begin(&renderingCommandBuffers[frameInFlightIndex], framebuffers[framebufferIndex].framebuffer, window->extent);

	for (Entity entity : entities) {
		auto const& renderable = ecs.getComponent<Renderable>(entity);
		std::string mapKey = renderable.vertexShaderPath + renderable.fragmentShaderPath + renderable.tesselationControlShaderPath + renderable.tesselationEvaluationShaderPath + renderable.geometryShaderPath;

		graphicsPipelines.at(mapKey).bind(&renderingCommandBuffers[frameInFlightIndex]);

		if (graphicsPipelines.at(mapKey).layoutBindings.size() != 0) {
			entityDescriptorSets.at(entity).at(frameInFlightIndex).bind(&renderingCommandBuffers[frameInFlightIndex]);
		}

		vkCmdBindVertexBuffers(renderingCommandBuffers[frameInFlightIndex].commandBuffer, 0, 1, &vertexBuffer.buffer, &offset);
		vkCmdBindIndexBuffer(renderingCommandBuffers[frameInFlightIndex].commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(renderingCommandBuffers[frameInFlightIndex].commandBuffer, 6, 1, 0, 0, 0);
	}

	renderPasses[0].end(&renderingCommandBuffers[frameInFlightIndex]);

	renderingCommandBuffers[frameInFlightIndex].end();
}

void Renderer::createResources() {
	// Swapchain
	swapchain.init(window, &swapchainSize);

	// Framebuffers
	Image colorAttachment;
	colorAttachment.allocationId = ImageTools::createImage(&colorAttachment.image, 1, window->extent.width, window->extent.height, 1, physicalDevice.maxUsableSampleCount, swapchain.surfaceFormat.format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	ImageTools::createImageView(&colorAttachment.imageView, colorAttachment.image, 1, 1, VK_IMAGE_VIEW_TYPE_2D, swapchain.surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::transitionLayout(colorAttachment.image, swapchain.surfaceFormat.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);
	colorImages.push_back(colorAttachment);
	Image depthAttachment;
	depthAttachment.allocationId = ImageTools::createImage(&depthAttachment.image, 1, window->extent.width, window->extent.height, 1, physicalDevice.maxUsableSampleCount, physicalDevice.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	ImageTools::createImageView(&depthAttachment.imageView, depthAttachment.image, 1, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	ImageTools::transitionLayout(depthAttachment.image, physicalDevice.depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);
	depthImages.push_back(depthAttachment);
	std::vector<std::vector<VkImageView>> framebufferAttachments;
	framebufferAttachments.resize(swapchainSize);
	framebuffers.resize(swapchainSize);
	for (int i = 0; i < framebufferAttachments.size(); i++) {
		framebufferAttachments[i].push_back(colorAttachment.imageView);
		framebufferAttachments[i].push_back(depthAttachment.imageView);
		framebufferAttachments[i].push_back(swapchain.imageViews[i]);
		framebuffers[i].init(&renderPasses[0], framebufferAttachments[i], window->extent.width, window->extent.height);
	}
}

void Renderer::destroyResources() {
	swapchain.destroy();
	for (Image& colorImage : colorImages) {
		colorImage.destroy();
	}
	colorImages.clear();
	colorImages.shrink_to_fit();
	for (Image& depthImage : depthImages) {
		depthImage.destroy();
	}
	depthImages.clear();
	depthImages.shrink_to_fit();
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.destroy();
	}
	framebuffers.clear();
	framebuffers.shrink_to_fit();
}
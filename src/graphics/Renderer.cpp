#include "Renderer.h"
#include "resources/RendererResources.h"
#include "resources/ShaderResources.h"
#include "../inputs/Inputs.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Light.h"
#include "../ecs/components/Camera.h"
#include "../ecs/components/Renderable.h"

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
	NEIGE_INFO("Color format : " + NeigeVKTranslate::vkFormatToString(physicalDevice.colorFormat));
	NEIGE_INFO("Depth format : " + NeigeVKTranslate::vkFormatToString(physicalDevice.depthFormat));

	// Viewports
	fullscreenViewport.init(window->extent.width, window->extent.height);

	// Render passes
	{
		std::vector<RenderPassAttachment> attachments;
		attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, physicalDevice.colorFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
		attachments.push_back(RenderPassAttachment(AttachmentType::DEPTH, physicalDevice.depthFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));
		
		std::vector<SubpassDependency> dependencies;
		dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });
		dependencies.push_back({ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT });
		dependencies.push_back({ VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT });

		RenderPass renderPass;
		renderPass.init(attachments, dependencies);
		renderPasses.emplace("scene", renderPass);
	}

	{
		std::vector<RenderPassAttachment> attachments;
		attachments.push_back(RenderPassAttachment(AttachmentType::COLOR, swapchain.surfaceFormat.format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));

		std::vector<SubpassDependency> dependencies;
		dependencies.push_back({ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0 });

		RenderPass renderPass;
		renderPass.init(attachments, dependencies);
		renderPasses.emplace("post", renderPass);
	}

	// Camera
	auto& cameraCamera = ecs.getComponent<Camera>(camera);
	cameraCamera.projection = Camera::createPerspectiveProjection(cameraCamera.FOV, window->extent.width / static_cast<float>(window->extent.height), cameraCamera.nearPlane, cameraCamera.farPlane, true);

	cameraBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (Buffer& buffer : cameraBuffers) {
		BufferTools::createUniformBuffer(buffer.buffer, buffer.deviceMemory, sizeof(CameraUniformBufferObject));
	}

	// Lights
	lightingBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (Buffer& buffer : lightingBuffers) {
		BufferTools::createUniformBuffer(buffer.buffer, buffer.deviceMemory, sizeof(LightingUniformBufferObject));
	}

	// Time
	timeBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (Buffer& buffer : timeBuffers) {
		BufferTools::createUniformBuffer(buffer.buffer, buffer.deviceMemory, sizeof(double));
	}

	// Depth prepass
	depthPrepass.init(fullscreenViewport);

	// SSAO
	ssao.init(fullscreenViewport);

	// Shadow
	shadow.init();

	// Envmap
	envmap.init(cameraCamera.envmapPath);

	skyboxGraphicsPipeline.vertexShaderPath = "../shaders/skybox.vert";
	skyboxGraphicsPipeline.fragmentShaderPath = "../shaders/skybox.frag";
	skyboxGraphicsPipeline.renderPass = &renderPasses.at("scene");
	skyboxGraphicsPipeline.multiSample = false;
	skyboxGraphicsPipeline.viewport = &fullscreenViewport;
	skyboxGraphicsPipeline.colorBlend = false;
	skyboxGraphicsPipeline.depthCompare = Compare::LESS_OR_EQUAL;
	skyboxGraphicsPipeline.init();

	skyboxDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		skyboxDescriptorSets[i].init(&skyboxGraphicsPipeline, 0);

		VkDescriptorBufferInfo cameraInfo = {};
		cameraInfo.buffer = cameraBuffers.at(i).buffer;
		cameraInfo.offset = 0;
		cameraInfo.range = sizeof(CameraUniformBufferObject);

		VkDescriptorImageInfo skyboxInfo = {};
		skyboxInfo.sampler = envmap.defaultSkybox.imageSampler;
		skyboxInfo.imageView = envmap.skyboxImage.imageView;
		skyboxInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector<VkWriteDescriptorSet> writesDescriptorSet;

		VkWriteDescriptorSet cameraWriteDescriptorSet = {};
		cameraWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		cameraWriteDescriptorSet.pNext = nullptr;
		cameraWriteDescriptorSet.dstSet = skyboxDescriptorSets[i].descriptorSet;
		cameraWriteDescriptorSet.dstBinding = 0;
		cameraWriteDescriptorSet.dstArrayElement = 0;
		cameraWriteDescriptorSet.descriptorCount = 1;
		cameraWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		cameraWriteDescriptorSet.pImageInfo = nullptr;
		cameraWriteDescriptorSet.pBufferInfo = &cameraInfo;
		cameraWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(cameraWriteDescriptorSet);

		VkWriteDescriptorSet skyboxWriteDescriptorSet = {};
		skyboxWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		skyboxWriteDescriptorSet.pNext = nullptr;
		skyboxWriteDescriptorSet.dstSet = skyboxDescriptorSets[i].descriptorSet;
		skyboxWriteDescriptorSet.dstBinding = 1;
		skyboxWriteDescriptorSet.dstArrayElement = 0;
		skyboxWriteDescriptorSet.descriptorCount = 1;
		skyboxWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		skyboxWriteDescriptorSet.pImageInfo = &skyboxInfo;
		skyboxWriteDescriptorSet.pBufferInfo = nullptr;
		skyboxWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(skyboxWriteDescriptorSet);

		skyboxDescriptorSets[i].update(writesDescriptorSet);
	}

	// Framebuffers
	createResources();

	{
		for (Entity light : lights) {
			auto const& lightLight = ecs.getComponent<Light>(light);

			if (lightLight.type == LightType::DIRECTIONAL || lightLight.type == LightType::SPOT) {
				std::vector<Framebuffer> lightFramebuffers;
				lightFramebuffers.resize(MAX_FRAMES_IN_FLIGHT);

				Image depthAttachment;
				ImageTools::createImage(&depthAttachment.image, 1, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthAttachment.allocationId);
				ImageTools::createImageView(&depthAttachment.imageView, depthAttachment.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
				shadow.images.push_back(depthAttachment);

				std::vector<std::vector<VkImageView>> framebufferAttachments;
				framebufferAttachments.resize(MAX_FRAMES_IN_FLIGHT);
				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
					framebufferAttachments[i].push_back(depthAttachment.imageView);
					lightFramebuffers[i].init(&shadow.renderPass, framebufferAttachments[i], SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 1);
				}
				shadow.framebuffers.push_back(lightFramebuffers);

				shadow.mapCount++;
			}
		}
	}

	// Post-process
	GraphicsPipeline postGraphicsPipeline;
	postGraphicsPipeline.vertexShaderPath = "../shaders/fullscreenTriangle.vert";
	postGraphicsPipeline.fragmentShaderPath = "../shaders/postProcess.frag";
	postGraphicsPipeline.renderPass = &renderPasses.at("post");
	postGraphicsPipeline.viewport = &fullscreenViewport;
	postGraphicsPipeline.multiSample = false;
	postGraphicsPipeline.colorBlend = false;
	postGraphicsPipeline.depthCompare = Compare::LESS;
	postGraphicsPipeline.backfaceCulling = false;
	postGraphicsPipeline.init();
	graphicsPipelines.emplace("post", postGraphicsPipeline);

	createPostProcessDescriptorSet();

	// Default textures
	float defaultDiffuse[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	Image defaultDiffuseImage;
	ImageTools::loadColor(defaultDiffuse, &defaultDiffuseImage.image, VK_FORMAT_R8G8B8A8_SRGB, &defaultDiffuseImage.mipmapLevels, &defaultDiffuseImage.allocationId);
	ImageTools::createImageView(&defaultDiffuseImage.imageView, defaultDiffuseImage.image, 0, 1, 0, defaultDiffuseImage.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&defaultDiffuseImage.imageSampler, defaultDiffuseImage.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	textures.emplace("defaultDiffuse", defaultDiffuseImage);

	float defaultNormal[4] = { 0.5f, 0.5f, 1.0f, 0.0f };
	Image defaultNormalImage;
	ImageTools::loadColor(defaultNormal, &defaultNormalImage.image, VK_FORMAT_R8G8B8A8_UNORM, &defaultNormalImage.mipmapLevels, &defaultNormalImage.allocationId);
	ImageTools::createImageView(&defaultNormalImage.imageView, defaultNormalImage.image, 0, 1, 0, defaultNormalImage.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&defaultNormalImage.imageSampler, defaultNormalImage.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	textures.emplace("defaultNormal", defaultNormalImage);

	float defaultMetallicRoughness[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Image defaultMetallicRoughnessImage;
	ImageTools::loadColor(defaultMetallicRoughness, &defaultMetallicRoughnessImage.image, VK_FORMAT_R8G8B8A8_UNORM, &defaultMetallicRoughnessImage.mipmapLevels, &defaultMetallicRoughnessImage.allocationId);
	ImageTools::createImageView(&defaultMetallicRoughnessImage.imageView, defaultMetallicRoughnessImage.image, 0, 1, 0, defaultMetallicRoughnessImage.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&defaultMetallicRoughnessImage.imageSampler, defaultMetallicRoughnessImage.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	textures.emplace("defaultMetallicRoughness", defaultMetallicRoughnessImage);

	float defaultEmissive[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Image defaultEmissiveImage;
	ImageTools::loadColor(defaultEmissive, &defaultEmissiveImage.image, VK_FORMAT_R8G8B8A8_UNORM, &defaultEmissiveImage.mipmapLevels, &defaultEmissiveImage.allocationId);
	ImageTools::createImageView(&defaultEmissiveImage.imageView, defaultEmissiveImage.image, 0, 1, 0, defaultEmissiveImage.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&defaultEmissiveImage.imageSampler, defaultEmissiveImage.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	textures.emplace("defaultEmissive", defaultEmissiveImage);

	float defaultOcclusion[4] = { 1.0, 1.0, 1.0, 0.0 };
	Image defaultOcclusionImage;
	ImageTools::loadColor(defaultOcclusion, &defaultOcclusionImage.image, VK_FORMAT_R8G8B8A8_UNORM, &defaultOcclusionImage.mipmapLevels, &defaultOcclusionImage.allocationId);
	ImageTools::createImageView(&defaultOcclusionImage.imageView, defaultOcclusionImage.image, 0, 1, 0, defaultOcclusionImage.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	ImageTools::createImageSampler(&defaultOcclusionImage.imageSampler, defaultOcclusionImage.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
	textures.emplace("defaultOcclusion", defaultOcclusionImage);

	Material defaultMaterial = { "defaultDiffuse", "defaultNormal", "defaultMetallicRoughness", "defaultEmissive", "defaultOcclusion" };
	materials.push_back(defaultMaterial);

	// Object resources
	for (Entity object : entities) {
		loadObject(object);
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
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		fences[i].init();
		IAsemaphores[i].init();
	}

	RFsemaphores.resize(swapchainSize);
	for (uint32_t i = 0; i < swapchainSize; i++) {
		RFsemaphores[i].init();
	}
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
				graphicsPipeline->init();
			}
			skyboxGraphicsPipeline.destroyPipeline();
			skyboxGraphicsPipeline.init();
		}

		if (keyboardInputs.cKey == KeyState::PRESSED) {
			memoryAllocator.memoryAnalyzer();
		}
	}

	if (window->gotResized) {
		window->gotResized = false;
		reloadOnResize();
	}

	fences[currentFrame].wait();

	uint32_t swapchainImage;
	VkResult result = swapchain.acquireNextImage(&IAsemaphores[currentFrame], &swapchainImage);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		reloadOnResize();
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
	submitInfo.pSignalSemaphores = &RFsemaphores[swapchainImage].semaphore;

	fences[currentFrame].reset();
	NEIGE_VK_CHECK(vkQueueSubmit(logicalDevice.queues.graphicsQueue, 1, &submitInfo, fences[currentFrame].fence));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &RFsemaphores[swapchainImage].semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.swapchain;
	presentInfo.pImageIndices = &swapchainImage;
	presentInfo.pResults = nullptr;
	result = vkQueuePresentKHR(logicalDevice.queues.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->gotResized) {
		window->gotResized = false;
		reloadOnResize();
	}
	else if (result != VK_SUCCESS) {
		NEIGE_ERROR("Unable to present image.");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::destroy() {
	logicalDevice.wait();
	destroyResources();
	depthPrepass.destroy();
	envmap.destroy();
	shadow.destroy();
	ssao.destroy();
	for (CommandPool& renderingCommandPool : renderingCommandPools) {
		renderingCommandPool.destroy();
	}
	for (std::unordered_map<std::string, RenderPass>::iterator it = renderPasses.begin(); it != renderPasses.end(); it++) {
		RenderPass* renderPass = &it->second;
		renderPass->destroy();
	}
	for (Buffer& buffer : cameraBuffers) {
		buffer.destroy();
	}
	for (Buffer& buffer : lightingBuffers) {
		buffer.destroy();
	}
	for (Buffer& buffer : timeBuffers) {
		buffer.destroy();
	}
	for (Entity entity : entities) {
		auto& objectRenderable = ecs.getComponent<Renderable>(entity);

		for (Buffer& buffer : objectRenderable.buffers) {
			buffer.destroy();
		}
	}
	for (std::unordered_map<std::string, GraphicsPipeline>::iterator it = graphicsPipelines.begin(); it != graphicsPipelines.end(); it++) {
		GraphicsPipeline* graphicsPipeline = &it->second;
		graphicsPipeline->destroy();
	}
	skyboxGraphicsPipeline.destroy();
	for (std::unordered_map<std::string, Shader>::iterator it = shaders.begin(); it != shaders.end(); it++) {
		Shader* shader = &it->second;
		shader->destroy();
	}
	for (std::unordered_map<std::string, Image>::iterator it = textures.begin(); it != textures.end(); it++) {
		Image* texture = &it->second;
		texture->destroy();
	}
	for (std::unordered_map<std::string, Model>::iterator it = models.begin(); it != models.end(); it++) {
		Model* model = &it->second;
		model->destroy();
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
	memoryAllocator.destroy();
	window->surface.destroy();
	logicalDevice.destroy();
	instance.destroy();
}

void Renderer::loadObject(Entity object) {
	auto& objectRenderable = ecs.getComponent<Renderable>(object);

	objectRenderable.createLookupString();

	// Graphics pipelines
	if (graphicsPipelines.find(objectRenderable.lookupString) == graphicsPipelines.end()) {
		GraphicsPipeline graphicsPipeline;
		graphicsPipeline.vertexShaderPath = objectRenderable.vertexShaderPath;
		graphicsPipeline.fragmentShaderPath = objectRenderable.fragmentShaderPath;
		graphicsPipeline.tesselationControlShaderPath = objectRenderable.tesselationControlShaderPath;
		graphicsPipeline.tesselationEvaluationShaderPath = objectRenderable.tesselationEvaluationShaderPath;
		graphicsPipeline.geometryShaderPath = objectRenderable.geometryShaderPath;
		graphicsPipeline.renderPass = &renderPasses.at("scene");
		graphicsPipeline.multiSample = false;
		graphicsPipeline.viewport = &fullscreenViewport;
		graphicsPipeline.topology = objectRenderable.topology;
		graphicsPipeline.colorBlend = false;
		graphicsPipeline.depthCompare = Compare::EQUAL;
		graphicsPipeline.depthWrite = false;
		graphicsPipeline.init();
		graphicsPipelines.emplace(objectRenderable.lookupString, graphicsPipeline);
	}

	objectRenderable.graphicsPipeline = &graphicsPipelines.at(objectRenderable.lookupString);

	objectRenderable.buffers.resize(MAX_FRAMES_IN_FLIGHT);
	objectRenderable.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	objectRenderable.depthPrepassDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	objectRenderable.shadowDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

	if (objectRenderable.graphicsPipeline->sets.size() != 0) {
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			// Buffers
			BufferTools::createUniformBuffer(objectRenderable.buffers.at(i).buffer, objectRenderable.buffers.at(i).deviceMemory, sizeof(ObjectUniformBufferObject));

			// Descriptor sets
			objectRenderable.createEntityDescriptorSet(i);

			// Depth prepass
			objectRenderable.createDepthPrepassEntityDescriptorSet(i);

			// Shadow
			objectRenderable.createShadowEntityDescriptorSet(i);
		}
	}

	// Model
	if (models.find(objectRenderable.modelPath) == models.end()) {
		Model model;
		model.init(objectRenderable.modelPath);
		models.emplace(objectRenderable.modelPath, model);
	}
	if (models.at(objectRenderable.modelPath).meshes.at(0).descriptorSets.find(objectRenderable.graphicsPipeline) == models.at(objectRenderable.modelPath).meshes.at(0).descriptorSets.end()) {
		models.at(objectRenderable.modelPath).createDescriptorSets(&graphicsPipelines.at(objectRenderable.lookupString));
	}
}

void Renderer::updateData(uint32_t frameInFlightIndex) {
	void* data;

	// Camera
	auto& cameraCamera = ecs.getComponent<Camera>(camera);

	CameraUniformBufferObject cubo = {};
	cameraCamera.view = Camera::createLookAtView(cameraCamera.position, cameraCamera.position + cameraCamera.to, glm::vec3(0.0f, 1.0f, 0.0f));
	cubo.view = cameraCamera.view;
	cubo.projection = cameraCamera.projection;
	cubo.position = cameraCamera.position;

	cameraBuffers.at(frameInFlightIndex).map(0, sizeof(CameraUniformBufferObject), &data);
	memcpy(data, &cubo, sizeof(CameraUniformBufferObject));
	cameraBuffers.at(frameInFlightIndex).unmap();

	// Lights
	int dirLightCount = 0;
	int pointLightCount = 0;
	int spotLightCount = 0;
	LightingUniformBufferObject lubo = {};
	ShadowUniformBufferObject subo = {};
	for (Entity entity : lights) {
		auto const& lightLight = ecs.getComponent<Light>(entity);

		if (lightLight.type == LightType::DIRECTIONAL) {
			lubo.dirLightsDirection[dirLightCount] = glm::vec4(lightLight.direction, 0.0f);
			lubo.dirLightsColor[dirLightCount] = glm::vec4(lightLight.color, 0.0f);

			glm::vec3 eye = -lightLight.direction;
			glm::vec3 up = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), eye) == (glm::length(glm::vec3(0.0f, 1.0f, 0.0f)) * glm::length(eye)) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0);
			glm::mat4 shadowProjection = Camera::createOrthoProjection(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
			glm::mat4 shadowView = Camera::createLookAtView(eye + cameraCamera.position + (cameraCamera.to * 3.0f), glm::vec3(0.0f, 0.0f, 0.0f) + cameraCamera.position + (cameraCamera.to * 3.0f), up);
			subo.dirLightSpaces[dirLightCount] = shadowProjection * shadowView;

			dirLightCount++;
		}
		else if (lightLight.type == LightType::POINT) {
			lubo.pointLightsPosition[pointLightCount] = glm::vec4(lightLight.position, 0.0f);
			lubo.pointLightsColor[pointLightCount] = glm::vec4(lightLight.color, 0.0f);

			pointLightCount++;
		}
		else if (lightLight.type == LightType::SPOT) {
			lubo.spotLightsPosition[spotLightCount] = glm::vec4(lightLight.position, 0.0f);
			lubo.spotLightsDirection[spotLightCount] = glm::vec4(lightLight.direction, 0.0f);
			lubo.spotLightsColor[spotLightCount] = glm::vec4(lightLight.color, 0.0f);
			lubo.spotLightsCutoffs[spotLightCount] = glm::vec4(glm::cos(glm::radians(lightLight.cutoffs.x)), glm::cos(glm::radians(lightLight.cutoffs.y)), 0.0f, 0.0f);

			glm::vec3 eye = lightLight.position;
			glm::vec3 to = lightLight.direction;
			glm::vec3 up = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), -to) == (glm::length(glm::vec3(0.0f, 1.0f, 0.0f)) * glm::length(-to)) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0);
			glm::mat4 shadowProjection = Camera::createPerspectiveProjection(120.0f, SHADOWMAP_WIDTH / static_cast<float>(SHADOWMAP_HEIGHT), 0.1f, 20.0f, false);
			glm::mat4 shadowView = Camera::createLookAtView(eye, eye + to, up);
			subo.spotLightSpaces[spotLightCount] = shadowProjection * shadowView;

			spotLightCount++;
		}
	}

	lubo.numLights.x = static_cast<float>(dirLightCount);
	lubo.numLights.y = static_cast<float>(pointLightCount);
	lubo.numLights.z = static_cast<float>(spotLightCount);

	subo.numLights.x = static_cast<float>(dirLightCount);
	subo.numLights.y = static_cast<float>(pointLightCount);
	subo.numLights.z = static_cast<float>(spotLightCount);

	lightingBuffers.at(frameInFlightIndex).map(0, sizeof(LightingUniformBufferObject), &data);
	memcpy(data, &lubo, sizeof(LightingUniformBufferObject));
	lightingBuffers.at(frameInFlightIndex).unmap();

	shadow.buffers.at(frameInFlightIndex).map(0, sizeof(ShadowUniformBufferObject), &data);
	memcpy(data, &subo, sizeof(ShadowUniformBufferObject));
	shadow.buffers.at(frameInFlightIndex).unmap();

	// Time
	float time = static_cast<float>(glfwGetTime());
	timeBuffers.at(frameInFlightIndex).map(0, sizeof(float), &data);
	memcpy(data, &time, sizeof(double));
	timeBuffers.at(frameInFlightIndex).unmap();

	// Renderables
	for (Entity object : entities) {
		auto const& objectTransform = ecs.getComponent<Transform>(object);
		auto& objectRenderable = ecs.getComponent<Renderable>(object);

		if (graphicsPipelines.at(objectRenderable.lookupString).sets.size() != 0) {
			ObjectUniformBufferObject oubo = {};
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), objectTransform.position);
			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(objectTransform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(objectTransform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(objectTransform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), objectTransform.scale);
			oubo.model = translate * rotateX * rotateY * rotateZ * scale;

			objectRenderable.buffers.at(frameInFlightIndex).map(0, sizeof(ObjectUniformBufferObject), &data);
			memcpy(data, &oubo, sizeof(ObjectUniformBufferObject));
			objectRenderable.buffers.at(frameInFlightIndex).unmap();
		}
	}
}

void Renderer::recordRenderingCommands(uint32_t frameInFlightIndex, uint32_t framebufferIndex) {
	currentPipeline = nullptr;

	RenderPass* sceneRenderPass = &renderPasses.at("scene");
	RenderPass* postRenderPass = &renderPasses.at("post");

	renderingCommandPools[frameInFlightIndex].reset();
	renderingCommandBuffers[frameInFlightIndex].begin();

	// Depth prepass
	depthPrepass.renderPass.begin(&renderingCommandBuffers[frameInFlightIndex], depthPrepass.framebuffers[frameInFlightIndex].framebuffer, window->extent);
	depthPrepass.graphicsPipeline.bind(&renderingCommandBuffers[frameInFlightIndex]);

	for (Entity object : entities) {
		auto& objectRenderable = ecs.getComponent<Renderable>(object);

		objectRenderable.depthPrepassDescriptorSets.at(frameInFlightIndex).bind(&renderingCommandBuffers[frameInFlightIndex], 0);

		models.at(objectRenderable.modelPath).draw(&renderingCommandBuffers[frameInFlightIndex], &depthPrepass.graphicsPipeline, frameInFlightIndex, false);
	}

	depthPrepass.renderPass.end(&renderingCommandBuffers[frameInFlightIndex]);

	// Shadow
	int lightIndex = 0;
	for (Entity light : lights) {
		auto const& lightLight = ecs.getComponent<Light>(light);

		if (lightLight.type == LightType::DIRECTIONAL || lightLight.type == LightType::SPOT) {
			shadow.renderPass.begin(&renderingCommandBuffers[frameInFlightIndex], shadow.framebuffers[lightIndex].at(frameInFlightIndex).framebuffer, { SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT });

			shadow.graphicsPipeline.bind(&renderingCommandBuffers[frameInFlightIndex]);
			shadow.graphicsPipeline.pushConstant(&renderingCommandBuffers[frameInFlightIndex], VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(int), &lightIndex);

			for (Entity object : entities) {
				auto& objectRenderable = ecs.getComponent<Renderable>(object);

				objectRenderable.shadowDescriptorSets.at(frameInFlightIndex).bind(&renderingCommandBuffers[frameInFlightIndex], 0);

				models.at(objectRenderable.modelPath).draw(&renderingCommandBuffers[frameInFlightIndex], &shadow.graphicsPipeline, frameInFlightIndex, false);
			}

			shadow.renderPass.end(&renderingCommandBuffers[frameInFlightIndex]);

			lightIndex++;
		}
	}

	// Scene
	sceneRenderPass->begin(&renderingCommandBuffers[frameInFlightIndex], sceneFramebuffers[frameInFlightIndex].framebuffer, window->extent);
	for (Entity object : entities) {
		auto& objectRenderable = ecs.getComponent<Renderable>(object);

		if (currentPipeline != objectRenderable.graphicsPipeline) {
			objectRenderable.graphicsPipeline->bind(&renderingCommandBuffers[frameInFlightIndex]);

			currentPipeline = objectRenderable.graphicsPipeline;
		}

		if (objectRenderable.graphicsPipeline->sets.size() != 0) {
			objectRenderable.descriptorSets.at(frameInFlightIndex).bind(&renderingCommandBuffers[frameInFlightIndex], 0);
		}

		models.at(objectRenderable.modelPath).draw(&renderingCommandBuffers[frameInFlightIndex], objectRenderable.graphicsPipeline, frameInFlightIndex, true);
	}
	skyboxGraphicsPipeline.bind(&renderingCommandBuffers[frameInFlightIndex]);
	skyboxDescriptorSets.at(frameInFlightIndex).bind(&renderingCommandBuffers[frameInFlightIndex], 0);

	envmap.draw(&renderingCommandBuffers[frameInFlightIndex]);

	sceneRenderPass->end(&renderingCommandBuffers[frameInFlightIndex]);

	// SSAO
	ssao.draw(&renderingCommandBuffers[frameInFlightIndex], frameInFlightIndex);

	// Post-processing
	postRenderPass->begin(&renderingCommandBuffers[frameInFlightIndex], postFramebuffers[framebufferIndex].framebuffer, window->extent);
	graphicsPipelines.at("post").bind(&renderingCommandBuffers[frameInFlightIndex]);
	postDescriptorSet.bind(&renderingCommandBuffers[frameInFlightIndex], 0);

	vkCmdDraw(renderingCommandBuffers[frameInFlightIndex].commandBuffer, 3, 1, 0, 0);

	postRenderPass->end(&renderingCommandBuffers[frameInFlightIndex]);

	renderingCommandBuffers[frameInFlightIndex].end();
}

void Renderer::createResources() {
	// Framebuffers
	{
		ImageTools::createImage(&colorImage.image, 1, window->extent.width, window->extent.height, 1, VK_SAMPLE_COUNT_1_BIT, physicalDevice.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &colorImage.allocationId);
		ImageTools::createImageView(&colorImage.imageView, colorImage.image, 0, 1, 0, 1, VK_IMAGE_VIEW_TYPE_2D, physicalDevice.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		ImageTools::createImageSampler(&colorImage.imageSampler, 2, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
		ImageTools::transitionLayout(colorImage.image, physicalDevice.colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1);
	
		std::vector<std::vector<VkImageView>> framebufferAttachments;
		framebufferAttachments.resize(swapchainSize);
		sceneFramebuffers.resize(swapchainSize);
		for (uint32_t i = 0; i < swapchainSize; i++) {
			framebufferAttachments[i].push_back(colorImage.imageView);
			framebufferAttachments[i].push_back(depthPrepass.image.imageView);
			sceneFramebuffers[i].init(&renderPasses.at("scene"), framebufferAttachments[i], window->extent.width, window->extent.height, 1);
		}
	}

	{
		std::vector<std::vector<VkImageView>> framebufferAttachments;
		framebufferAttachments.resize(swapchainSize);
		postFramebuffers.resize(swapchainSize);
		for (uint32_t i = 0; i < swapchainSize; i++) {
			framebufferAttachments[i].push_back(swapchain.imageViews[i]);
			postFramebuffers[i].init(&renderPasses.at("post"), framebufferAttachments[i], window->extent.width, window->extent.height, 1);
		}
	}
}

void Renderer::destroyResources() {
	swapchain.destroy();
	colorImage.destroy();
	for (Framebuffer& framebuffer : sceneFramebuffers) {
		framebuffer.destroy();
	}
	for (Framebuffer& framebuffer : postFramebuffers) {
		framebuffer.destroy();
	}
	sceneFramebuffers.clear();
	sceneFramebuffers.shrink_to_fit();
	depthPrepass.destroyResources();
	ssao.destroyResources();
}

void Renderer::createPostProcessDescriptorSet() {
	postDescriptorSet.init(&graphicsPipelines.at("post"), 0);

	VkDescriptorImageInfo sceneInfo = {};
	sceneInfo.sampler = colorImage.imageSampler;
	sceneInfo.imageView = colorImage.imageView;
	sceneInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkDescriptorImageInfo ssaoInfo = {};
	ssaoInfo.sampler = ssao.ssaoBlurredImage.imageSampler;
	ssaoInfo.imageView = ssao.ssaoBlurredImage.imageView;
	ssaoInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	std::vector<VkWriteDescriptorSet> writesDescriptorSet;

	VkWriteDescriptorSet sceneWriteDescriptorSet = {};
	sceneWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	sceneWriteDescriptorSet.pNext = nullptr;
	sceneWriteDescriptorSet.dstSet = postDescriptorSet.descriptorSet;
	sceneWriteDescriptorSet.dstBinding = 0;
	sceneWriteDescriptorSet.dstArrayElement = 0;
	sceneWriteDescriptorSet.descriptorCount = 1;
	sceneWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sceneWriteDescriptorSet.pImageInfo = &sceneInfo;
	sceneWriteDescriptorSet.pBufferInfo = nullptr;
	sceneWriteDescriptorSet.pTexelBufferView = nullptr;
	writesDescriptorSet.push_back(sceneWriteDescriptorSet);

	VkWriteDescriptorSet ssaoWriteDescriptorSet = {};
	ssaoWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	ssaoWriteDescriptorSet.pNext = nullptr;
	ssaoWriteDescriptorSet.dstSet = postDescriptorSet.descriptorSet;
	ssaoWriteDescriptorSet.dstBinding = 1;
	ssaoWriteDescriptorSet.dstArrayElement = 0;
	ssaoWriteDescriptorSet.descriptorCount = 1;
	ssaoWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	ssaoWriteDescriptorSet.pImageInfo = &ssaoInfo;
	ssaoWriteDescriptorSet.pBufferInfo = nullptr;
	ssaoWriteDescriptorSet.pTexelBufferView = nullptr;
	writesDescriptorSet.push_back(ssaoWriteDescriptorSet);

	postDescriptorSet.update(writesDescriptorSet);
}

void Renderer::reloadOnResize() {
	while (window->extent.width == 0 || window->extent.height == 0) {
		window->waitEvents();
	}
	logicalDevice.wait();

	destroyResources();

	// Swapchain
	swapchain.init(window, &swapchainSize);

	fullscreenViewport.viewport.width = static_cast<float>(window->extent.width);
	fullscreenViewport.viewport.height = static_cast<float>(window->extent.height);
	fullscreenViewport.scissor.extent.width = window->extent.width;
	fullscreenViewport.scissor.extent.height = window->extent.height;

	// Depth prepass
	depthPrepass.createResources(fullscreenViewport);

	// SSAO
	ssao.createResources(fullscreenViewport);

	createResources();

	createPostProcessDescriptorSet();

	auto& cameraCamera = ecs.getComponent<Camera>(camera);
	cameraCamera.projection = Camera::createPerspectiveProjection(cameraCamera.FOV, window->extent.width / static_cast<float>(window->extent.height), cameraCamera.nearPlane, cameraCamera.farPlane, true);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		for (Entity entity : entities) {
			auto& entityRenderable = ecs.getComponent<Renderable>(entity);

			entityRenderable.createEntityDescriptorSet(i);
		}
	}
}

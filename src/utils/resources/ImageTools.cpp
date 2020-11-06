#include "ImageTools.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb/stb_image.h"
#include "../../graphics/resources/RendererResources.h"

void ImageTools::createImage(VkImage* image,
	uint32_t arrayLayers,
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels,
	VkSampleCountFlagBits msaaSamples,
	VkFormat format,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags memoryProperties,
	VkDeviceSize* allocationId) {
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = arrayLayers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = mipLevels;
	imageCreateInfo.arrayLayers = arrayLayers;
	imageCreateInfo.samples = msaaSamples;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	NEIGE_VK_CHECK(vkCreateImage(logicalDevice.device, &imageCreateInfo, nullptr, image));

	*allocationId = memoryAllocator.allocate(image, memoryProperties);
}

void ImageTools::createImageView(VkImageView* imageView,
	VkImage image,
	uint32_t baseArrayLayer,
	uint32_t arrayLayers,
	uint32_t baseMipLevel,
	uint32_t mipLevels,
	VkImageViewType viewType,
	VkFormat format,
	VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = viewType;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
	imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageViewCreateInfo.subresourceRange.layerCount = arrayLayers;
	NEIGE_VK_CHECK(vkCreateImageView(logicalDevice.device, &imageViewCreateInfo, nullptr, imageView));
}

void ImageTools::createImageSampler(VkSampler* sampler,
	uint32_t mipLevels,
	VkFilter filter,
	VkSamplerAddressMode addressMode,
	VkBorderColor borderColor) {
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = nullptr;
	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = filter;
	samplerCreateInfo.minFilter = filter;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = addressMode;
	samplerCreateInfo.addressModeV = addressMode;
	samplerCreateInfo.addressModeW = addressMode;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = 16.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = static_cast<float>(mipLevels);
	samplerCreateInfo.borderColor = borderColor;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	NEIGE_VK_CHECK(vkCreateSampler(logicalDevice.device, &samplerCreateInfo, nullptr, sampler));
}

void ImageTools::loadImage(const std::string& filePath,
	VkImage* imageDestination,
	VkFormat format,
	uint32_t* mipLevels,
	VkDeviceSize* allocationId) {
	int width;
	int height;
	int texChannels;

	stbi_uc* pixels = stbi_load(filePath.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
	VkDeviceSize size = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * 4 * sizeof(uint8_t);
	if (!pixels) {
		NEIGE_ERROR("Error with image file \"" + filePath + "\".");
	}

	*mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

	Buffer stagingBuffer;
	BufferTools::createStagingBuffer(stagingBuffer.buffer, stagingBuffer.deviceMemory, size);
	void* data;
	stagingBuffer.map(0, size, &data);
	memcpy(data, pixels, size);
	stagingBuffer.unmap();

	createImage(imageDestination, 1, width, height, *mipLevels, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, allocationId);
	transitionLayout(*imageDestination, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, *mipLevels, 1);
	BufferTools::copyToImage(stagingBuffer.buffer, *imageDestination, width, height, 1, sizeof(uint8_t));
	generateMipmaps(*imageDestination, format, width, height, *mipLevels, 1);

	stagingBuffer.destroy();
	stbi_image_free(pixels);
}

void ImageTools::loadHDREnvmap(const std::string& filePath,
	VkImage* imageDestination,
	VkFormat format,
	VkDeviceSize* allocationId) {
	if (FileTools::extension(filePath) != "hdr") {
		NEIGE_ERROR("Envmap file must be a \".hdr\" picture.");
	}

	int width;
	int height;
	int texChannels;

	stbi_set_flip_vertically_on_load(true);
	float* pixels = stbi_loadf(filePath.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
	stbi_set_flip_vertically_on_load(false);
	VkDeviceSize size = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * 4 * sizeof(float);
	if (!pixels) {
		NEIGE_ERROR("Error with hdr image file \"" + filePath + "\".");
	}

	Buffer stagingBuffer;
	BufferTools::createStagingBuffer(stagingBuffer.buffer, stagingBuffer.deviceMemory, size);
	void* data;
	stagingBuffer.map(0, size, &data);
	memcpy(data, pixels, size);
	stagingBuffer.unmap();

	createImage(imageDestination, 1, width, height, 1, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, allocationId);
	transitionLayout(*imageDestination, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);
	BufferTools::copyToImage(stagingBuffer.buffer, *imageDestination, width, height, 1, sizeof(float));
	transitionLayout(*imageDestination, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1);

	stagingBuffer.destroy();
	stbi_image_free(pixels);
}

void ImageTools::loadColor(float* color,
	VkImage* imageDestination,
	VkFormat format,
	uint32_t* mipLevels,
	VkDeviceSize* allocationId) {
	uint8_t r = static_cast<uint8_t>(round(255.0f * color[0]));
	uint8_t g = static_cast<uint8_t>(round(255.0f * color[1]));
	uint8_t b = static_cast<uint8_t>(round(255.0f * color[2]));
	uint8_t a = static_cast<uint8_t>(round(255.0f * color[3]));
	std::array<uint8_t, 4> colorData = { r, g, b, a };

	*mipLevels = 1;

	Buffer stagingBuffer;
	BufferTools::createStagingBuffer(stagingBuffer.buffer, stagingBuffer.deviceMemory, 4 * sizeof(uint8_t));
	void* pixelData;
	stagingBuffer.map(0, 4 * sizeof(uint8_t), &pixelData);
	memcpy(pixelData, colorData.data(), 4 * sizeof(uint8_t));
	stagingBuffer.unmap();

	createImage(imageDestination, 1, 1, 1, 1, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, allocationId);
	transitionLayout(*imageDestination, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);
	BufferTools::copyToImage(stagingBuffer.buffer, *imageDestination, 1, 1, 1, sizeof(uint8_t));
	transitionLayout(*imageDestination, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, *mipLevels, 1);

	stagingBuffer.destroy();
}

void ImageTools::loadColorForEnvmap(float* color,
	VkImage* imageDestination,
	VkFormat format,
	uint32_t* mipLevels,
	VkDeviceSize* allocationId) {
	*mipLevels = 1;

	Buffer stagingBuffer;
	BufferTools::createStagingBuffer(stagingBuffer.buffer, stagingBuffer.deviceMemory, 4 * sizeof(float));
	void* pixelData;
	stagingBuffer.map(0, 4 * sizeof(float), &pixelData);
	memcpy(pixelData, color, 4 * sizeof(float));
	stagingBuffer.unmap();

	createImage(imageDestination, 1, 1, 1, 1, VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, allocationId);
	transitionLayout(*imageDestination, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);
	BufferTools::copyToImage(stagingBuffer.buffer, *imageDestination, 1, 1, 1, sizeof(float));
	transitionLayout(*imageDestination, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, *mipLevels, 1);

	stagingBuffer.destroy();
}

void ImageTools::transitionLayout(VkImage image,
	VkFormat format,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	uint32_t mipLevels,
	uint32_t arrayLayers) {
	CommandPool commandPool;
	commandPool.init();
	CommandBuffer commandBuffer;
	commandBuffer.init(&commandPool);
	commandBuffer.begin();

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.oldLayout = oldLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT) {
			imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = arrayLayers;
	VkPipelineStageFlags srcPipelineStageFlags;
	VkPipelineStageFlags dstPipelineStageFlags;
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		srcPipelineStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstPipelineStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcPipelineStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstPipelineStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcPipelineStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstPipelineStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcPipelineStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstPipelineStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		srcPipelineStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstPipelineStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		srcPipelineStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstPipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else {
		NEIGE_ERROR("Unsupported image layout transition.");
	}
	vkCmdPipelineBarrier(commandBuffer.commandBuffer, srcPipelineStageFlags, dstPipelineStageFlags, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	commandBuffer.endAndSubmit();
	commandPool.destroy();
}

void ImageTools::generateMipmaps(VkImage image,
	VkFormat format,
	int32_t texelWidth,
	int32_t texelHeight,
	uint32_t mipLevels,
	uint32_t layers) {
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice.device, format, &formatProperties);
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		NEIGE_ERROR("Image format does not support automatic mipmapping.");
	}
	CommandPool commandPool;
	commandPool.init();
	CommandBuffer commandBuffer;
	commandBuffer.init(&commandPool);
	commandBuffer.begin();

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = layers;
	int mipWidth = texelWidth;
	int mipHeight = texelHeight;
	for (uint32_t i = 1; i < mipLevels; i++) {
		imageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		vkCmdPipelineBarrier(commandBuffer.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
		VkImageBlit imageBlit = {};
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.mipLevel = i - 1;
		imageBlit.srcSubresource.baseArrayLayer = 0;
		imageBlit.srcSubresource.layerCount = 1;
		imageBlit.srcOffsets[0] = { 0, 0, 0 };
		imageBlit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.mipLevel = i;
		imageBlit.dstSubresource.baseArrayLayer = 0;
		imageBlit.dstSubresource.layerCount = 1;
		imageBlit.dstOffsets[0] = { 0, 0, 0 };
		imageBlit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		vkCmdBlitImage(commandBuffer.commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vkCmdPipelineBarrier(commandBuffer.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
		mipWidth = mipWidth > 1 ? mipWidth / 2 : 1;
		mipHeight = mipHeight > 1 ? mipHeight / 2 : 1;
	}
	imageMemoryBarrier.subresourceRange.baseMipLevel = mipLevels - 1;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkCmdPipelineBarrier(commandBuffer.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	commandBuffer.endAndSubmit();
	commandPool.destroy();
}
#include "PhysicalDevice.h"

bool PhysicalDevice::isSuitable(const Surface* surface) {
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
	findQueueFamilies(surface->surface);
	if (queueFamilyIndices.isComplete()) {
		if (extensionSupport()) {
			SwapchainSupport deviceSwapchainSupport = swapchainSupport(surface->surface);
			return !deviceSwapchainSupport.formats.empty() && !deviceSwapchainSupport.presentModes.empty() && features.fillModeNonSolid && features.samplerAnisotropy && features.sampleRateShading;
		}
	}
	return false;
}

void PhysicalDevice::findQueueFamilies(VkSurfaceKHR surface) {
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());
	int queueIndex = 0;
	for (const auto& queueFamily : queueFamilyProperties) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamilyIndices.graphicsFamily = queueIndex;
		}
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			queueFamilyIndices.computeFamily = queueIndex;
		}
		VkBool32 presentSupport;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, queueIndex, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			queueFamilyIndices.presentFamily = queueIndex;
		}
		if (queueFamilyIndices.isComplete()) {
			break;
		}
		queueIndex++;
	}
}

bool PhysicalDevice::extensionSupport() {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensionProperties.data());
	std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
	for (const auto& extension : extensionProperties) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void PhysicalDevice::getMaxUsableSampleCount() {
	VkSampleCountFlags counts = std::min(properties.limits.framebufferColorSampleCounts, properties.limits.framebufferDepthSampleCounts);
	if (counts & VK_SAMPLE_COUNT_64_BIT) maxUsableSampleCount = VK_SAMPLE_COUNT_64_BIT;
	else if (counts & VK_SAMPLE_COUNT_32_BIT) maxUsableSampleCount = VK_SAMPLE_COUNT_32_BIT;
	else if (counts & VK_SAMPLE_COUNT_16_BIT) maxUsableSampleCount = VK_SAMPLE_COUNT_16_BIT;
	else if (counts & VK_SAMPLE_COUNT_8_BIT) maxUsableSampleCount = VK_SAMPLE_COUNT_8_BIT;
	else if (counts & VK_SAMPLE_COUNT_4_BIT) maxUsableSampleCount = VK_SAMPLE_COUNT_4_BIT;
	else if (counts & VK_SAMPLE_COUNT_2_BIT) maxUsableSampleCount = VK_SAMPLE_COUNT_2_BIT;
	else maxUsableSampleCount = VK_SAMPLE_COUNT_1_BIT;
}

SwapchainSupport PhysicalDevice::swapchainSupport(VkSurfaceKHR surface){
	SwapchainSupport swapchainSupport;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchainSupport.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		swapchainSupport.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapchainSupport.formats.data());
	}
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		swapchainSupport.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, swapchainSupport.presentModes.data());
	}

	return swapchainSupport;
}

void PhysicalDevice::findColorFormat() {
	std::vector<VkFormat> colorFormats;
	colorFormats.push_back(VK_FORMAT_R32G32B32A32_SFLOAT);
	colorFormats.push_back(VK_FORMAT_R16G16B16A16_SFLOAT);
	colorFormats.push_back(VK_FORMAT_B8G8R8A8_SRGB);

	for (VkFormat format : colorFormats) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(device, format, &formatProperties);
		if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
			colorFormat = format;
			break;
		}
	}
	NEIGE_ASSERT(colorFormat != VK_FORMAT_UNDEFINED, "Unable to find a suitable color format.");
}

void PhysicalDevice::findDepthFormat() {
	std::vector<VkFormat> depthFormats;
	depthFormats.push_back(VK_FORMAT_D32_SFLOAT);
	depthFormats.push_back(VK_FORMAT_D32_SFLOAT_S8_UINT);
	depthFormats.push_back(VK_FORMAT_D24_UNORM_S8_UINT);
	for (VkFormat format : depthFormats) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(device, format, &formatProperties);
		if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			depthFormat = format;
			break;
		}
	}
	NEIGE_ASSERT(depthFormat != VK_FORMAT_UNDEFINED, "Unable to find a suitable depth format.");
}

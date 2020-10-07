#pragma once
#include <vulkan\vulkan.hpp>
#include "NeigeTools.h"
#include <set>
#include <string>

struct PhysicalDevice {
	VkPhysicalDevice device;

	VkPhysicalDeviceProperties getProperties() {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);
		return properties;
	}

	VkPhysicalDeviceFeatures getFeatures() {
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(device, &features);
		return features;
	}

	bool isSuitable(VkSurfaceKHR surface) {
		VkPhysicalDeviceFeatures physicalDeviceFeatures = getFeatures();
		if (findQueueFamilies(surface).isComplete()) {
			if (extensionSupport()) {
				SwapchainSupport deviceSwapchainSupport = swapchainSupport(surface);
				return !deviceSwapchainSupport.formats.empty() && !deviceSwapchainSupport.presentModes.empty() && physicalDeviceFeatures.samplerAnisotropy;
			}
		}
		return false;
	}

	QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface) {
		QueueFamilyIndices queueFamilyIndices;
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

		return queueFamilyIndices;
	}

	bool extensionSupport() {
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

	SwapchainSupport swapchainSupport(VkSurfaceKHR surface) {
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
};
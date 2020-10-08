#pragma once
#include <vulkan/vulkan.hpp>
#include "NeigeTools.h"
#include <optional>

// Layers
const std::vector<const char*> layers = {
	"VK_LAYER_KHRONOS_validation"
};

// Extensions
const std::vector<const char*> extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Queue family indices
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> computeFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && computeFamily.has_value() && presentFamily.has_value();
	}
};

// Queue families
struct Queues {
	VkQueue graphicsQueue;
	VkQueue computeQueue;
	VkQueue presentQueue;
};

// Swapchain support
struct SwapchainSupport {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	VkExtent2D extent(VkExtent2D extent) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent = { std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, static_cast<uint32_t>(extent.width))),
				std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, static_cast<uint32_t>(extent.height))) };
			return actualExtent;
		}
	}

	VkSurfaceFormatKHR surfaceFormat() {
		for (const auto& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
				NEIGE_INFO("Swapchain format : VK_FORMAT_B8G8R8A8_SRGB");
				NEIGE_INFO("Swapchain color space : VK_COLORSPACE_SRGB_NONLINEAR_KHR");
				return format;
			}
		}
		NEIGE_WARNING("Could not find a suitable swapchain format.");
		return formats[0];
	}

	VkPresentModeKHR presentMode() {
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				NEIGE_INFO("Present mode : VK_PRESENT_MODE_MAILBOX_KHR");
				return presentMode;
			}
			else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				NEIGE_INFO("Present mode : VK_PRESENT_MODE_IMMEDIATE_KHR");
				return presentMode;
			}
		}
		NEIGE_INFO("Present mode : VK_PRESENT_MODE_FIFO_KHR");
		return VK_PRESENT_MODE_FIFO_KHR;
	}
};
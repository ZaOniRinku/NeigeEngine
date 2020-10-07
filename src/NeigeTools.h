#pragma once
#include <vulkan/vulkan.hpp>
#include "../external/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <string>
#include <optional>

#define NEIGE_VK_CHECK(f) \
	do { \
		VkResult check = f; \
		if (check) { \
			std::cerr << "\033[1m\033[31mVULKAN ERROR : \033[39m\033[0m A Vulkan error has happened." << std::endl << "File : " << __FILE__ << std::endl << "Function : " << __FUNCTION__ << std::endl << "Line : " << __LINE__ << std::endl; \
			exit(1); \
		} \
	} while (0)

#define NEIGE_WARNING(m) \
	do { \
		std::cerr << "\033[1m\033[33mNEIGE WARNING : \033[39m\033[0m" << m << std::endl; \
	} while(0)

#define NEIGE_ERROR(m) \
	do { \
		std::cerr << "\033[1m\033[31mNEIGE ERROR : \033[39m\033[0m" << m << std::endl; \
		exit(2); \
	} while(0)

#ifndef NDEBUG
#define NEIGE_INFO(m) \
	do { \
		std::cout << "\033[1m\033[36mNEIGE INFO : \033[39m\033[0m" << m << std::endl; \
	} while(0)
#else
#define NEIGE_INFO(m) \
	do { \
	} while(0)
#endif

#ifndef NDEBUG
#define NEIGE_ASSERT(c, m) \
		do { \
			if (!c) { \
				std::cerr << "\033[1m\033[35mNEIGE ASSERT : \033[39m\033[0m" << m << std::endl; \
				exit(3); \
			} \
		} while(0)
#else
#define NEIGE_ASSERT(c, m) \
		do { \
		} while(0)
#endif

#ifndef NDEBUG
const bool NEIGE_DEBUG = true;
#else
const bool NEIGE_DEBUG = false;
#endif

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

// Window extent
struct WindowExtent {
	int width;
	int height;
};

// Swapchain support
struct SwapchainSupport {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	VkExtent2D extent(WindowExtent windowExtent) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent = { std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, static_cast<uint32_t>(windowExtent.width))),
				std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, static_cast<uint32_t>(windowExtent.height))) };
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
			} else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				NEIGE_INFO("Present mode : VK_PRESENT_MODE_IMMEDIATE_KHR");
				return presentMode;
			}
		}
		NEIGE_INFO("Present mode : VK_PRESENT_MODE_FIFO_KHR");
		return VK_PRESENT_MODE_FIFO_KHR;
	}
};
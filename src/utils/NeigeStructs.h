#pragma once
#include "vulkan/vulkan.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../external/glm/glm/glm.hpp"
#include "NeigeDefines.h"
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

// Subpass dependency
struct SubpassDependency {
	VkPipelineStageFlags srcStageMask;
	VkPipelineStageFlags dstStageMask;
	VkAccessFlags srcAccessMask;
	VkAccessFlags dstAccessMask;
	VkDependencyFlags dependencyFlags;
};

// Vertex
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;

	static VkVertexInputBindingDescription getInputBindingDescription() {
		VkVertexInputBindingDescription inputBindingDescription = {};
		inputBindingDescription.binding = 0;
		inputBindingDescription.stride = sizeof(Vertex);
		inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return inputBindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions;
		VkVertexInputAttributeDescription positionAttribute = {};
		positionAttribute.binding = 0;
		positionAttribute.location = 0;
		positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionAttribute.offset = offsetof(Vertex, position);
		inputAttributeDescriptions.push_back(positionAttribute);

		VkVertexInputAttributeDescription normalAttribute = {};
		normalAttribute.binding = 0;
		normalAttribute.location = 1;
		normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalAttribute.offset = offsetof(Vertex, normal);
		inputAttributeDescriptions.push_back(normalAttribute);

		VkVertexInputAttributeDescription uvAttribute = {};
		uvAttribute.binding = 0;
		uvAttribute.location = 2;
		uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		uvAttribute.offset = offsetof(Vertex, uv);
		inputAttributeDescriptions.push_back(uvAttribute);

		VkVertexInputAttributeDescription tangentAttribute = {};
		tangentAttribute.binding = 0;
		tangentAttribute.location = 3;
		tangentAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		tangentAttribute.offset = offsetof(Vertex, tangent);
		inputAttributeDescriptions.push_back(tangentAttribute);

		return inputAttributeDescriptions;
	}
};
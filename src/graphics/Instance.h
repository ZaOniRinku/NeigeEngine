#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"

// Debug messenger callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	NEIGE_VK_VALIDATION_ERROR(pCallbackData->pMessage);

	return VK_FALSE;
}

struct Instance {
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

	void init(uint32_t engineVersion, const std::vector<const char*> windowExtensions);
	void destroy();
	void createDebugMessenger();
	void destroyDebugMessenger();
};
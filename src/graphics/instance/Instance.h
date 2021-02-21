#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "../../utils/structs/RendererStructs.h"

struct Instance {
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

	void init(uint32_t engineVersion, const std::vector<const char*> windowExtensions);
	void destroy();
	void createDebugMessenger();
	void destroyDebugMessenger();
};
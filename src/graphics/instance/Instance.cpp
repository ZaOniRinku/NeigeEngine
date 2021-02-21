#include "Instance.h"

// Debug messenger callback
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	[[maybe_unused]] void* pUserData) {
	NEIGE_VK_VALIDATION_ERROR(pCallbackData->pMessage);

	return VK_FALSE;
}

void Instance::init(uint32_t engineVersion, const std::vector<const char*> windowExtensions) {
	// Application
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = 0;
	applicationInfo.pApplicationName = "";
	applicationInfo.pEngineName = "NeigeEngine";
	applicationInfo.engineVersion = engineVersion;
	applicationInfo.apiVersion = VK_VERSION_1_2;

	// Layers
	if (NEIGE_DEBUG) {
		uint32_t propertyCount;
		NEIGE_VK_CHECK(vkEnumerateInstanceLayerProperties(&propertyCount, nullptr));
		std::vector<VkLayerProperties> properties(propertyCount);
		NEIGE_VK_CHECK(vkEnumerateInstanceLayerProperties(&propertyCount, properties.data()));

		bool validationLayerFound;
		for (const auto& layer : explicitLayers) {
			validationLayerFound = false;
			for (const VkLayerProperties& availableLayer : properties) {
				if (strcmp(availableLayer.layerName, layer) == 0) {
					validationLayerFound = true;
					break;
				}
			}
			if (!validationLayerFound) {
				NEIGE_WARNING("Layer " + std::string(layer) + " has not been found.");
			}
		}
	}

	// Instance
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	if (NEIGE_DEBUG) {
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(explicitLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = explicitLayers.data();
	}
	else {
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;
	}
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(windowExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = windowExtensions.data();
	NEIGE_VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

	if (NEIGE_DEBUG) {
		createDebugMessenger();
	}
}

void Instance::destroy() {
	if (NEIGE_DEBUG) {
		destroyDebugMessenger();
	}

	vkDestroyInstance(instance, nullptr);
}

void Instance::createDebugMessenger() {
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
	debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessengerCreateInfo.pNext = nullptr;
	debugMessengerCreateInfo.flags = 0;
	debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugMessengerCreateInfo.pfnUserCallback = debugCallback;
	debugMessengerCreateInfo.pUserData = nullptr;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	NEIGE_VK_CHECK(func(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger));
}

void Instance::destroyDebugMessenger() {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	func(instance, debugMessenger, nullptr);
}
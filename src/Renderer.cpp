#include "Renderer.h"

void Renderer::init() {
	// Application
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = 0;
	applicationInfo.pApplicationName = "";
	applicationInfo.pEngineName = "NeigeEngine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.apiVersion = VK_VERSION_1_2;

	// Layers
	if (NEIGE_DEBUG) {
		uint32_t propertyCount;
		NEIGE_VK_CHECK(vkEnumerateInstanceLayerProperties(&propertyCount, nullptr));
		std::vector<VkLayerProperties> properties(propertyCount);
		NEIGE_VK_CHECK(vkEnumerateInstanceLayerProperties(&propertyCount, properties.data()));

		bool validationLayerFound;
		for (const auto& layer : layers) {
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
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		instanceCreateInfo.ppEnabledLayerNames = layers.data();
	} else {
		instanceCreateInfo.enabledLayerCount = 0;
	}
	auto extensions = window.instanceExtensions();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
	NEIGE_VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

	// Debug messenger
	if (NEIGE_DEBUG) {
		createDebugMessenger();
	}
	
	// Surface
	window.createSurface(&instance, &surface);

	// Pick physical device
	VkPhysicalDevice preferredDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceType preferredDeviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		NEIGE_ERROR("Unable to find any GPU supporting Vulkan.");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const auto& device : devices) {
		if (physicalDeviceSuitable(device)) {
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				// GPU
				preferredDevice = device;
				preferredDeviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
			}
			else if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
				// Chipset
				if (preferredDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
					preferredDevice = device;
					preferredDeviceType = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
				}
			}
		}
	}
	physicalDevice = preferredDevice;
	NEIGE_ASSERT(physicalDevice != VK_NULL_HANDLE, "Unable to find a suitable GPU.");
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	NEIGE_INFO(properties.deviceName);
}

void Renderer::destroy() {
	if (NEIGE_DEBUG) {
		destroyDebugMessenger();
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void Renderer::createDebugMessenger() {
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
	debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessengerCreateInfo.pNext = nullptr;
	debugMessengerCreateInfo.flags = 0;
	debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugMessengerCreateInfo.pfnUserCallback = debugCallback;
	debugMessengerCreateInfo.pUserData = nullptr;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	NEIGE_VK_CHECK(func(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger));
}

void Renderer::destroyDebugMessenger() {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	func(instance, debugMessenger, nullptr);
}

bool Renderer::physicalDeviceSuitable(VkPhysicalDevice physicalDevice) {
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
	if (physicalDeviceFindQueueFamilies(physicalDevice).isComplete()) {
		if (physicalDeviceExtensionSupport(physicalDevice)) {
			SwapchainSupport swapchainSupport = physicalDeviceSwapchainSupport(physicalDevice);
			return !swapchainSupport.formats.empty() && !swapchainSupport.presentMode.empty() && physicalDeviceFeatures.samplerAnisotropy;
		}
	}
	return false;
}

QueueFamiliesIndices Renderer::physicalDeviceFindQueueFamilies(VkPhysicalDevice physicalDevice) {
	QueueFamiliesIndices queueFamiliesIndices;
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
	int queueIndex = 0;
	for (const auto& queueFamily : queueFamilyProperties) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamiliesIndices.graphicsFamily = queueIndex;
		}
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			queueFamiliesIndices.computeFamily = queueIndex;
		}
		VkBool32 presentSupport;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueIndex, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			queueFamiliesIndices.presentFamily = queueIndex;
		}
		if (queueFamiliesIndices.isComplete()) {
			break;
		}
		queueIndex++;
	}

	return queueFamiliesIndices;
}

bool Renderer::physicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties.data());
	std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
	for (const auto& extension : extensionProperties) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapchainSupport Renderer::physicalDeviceSwapchainSupport(VkPhysicalDevice physicalDevice) {
	SwapchainSupport swapchainSupport;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupport.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		swapchainSupport.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupport.formats.data());
	}
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		swapchainSupport.presentMode.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, swapchainSupport.presentMode.data());
	}

	return swapchainSupport;
}

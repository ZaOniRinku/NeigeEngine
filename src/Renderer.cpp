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
	auto instanceExtensions = window.instanceExtensions();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	NEIGE_VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

	// Debug messenger
	if (NEIGE_DEBUG) {
		createDebugMessenger();
	}
	
	// Surface
	window.createSurface(instance, &surface);

	// Pick physical device
	PhysicalDevice preferredDevice = { VK_NULL_HANDLE };
	VkPhysicalDeviceType preferredDeviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		NEIGE_ERROR("Unable to find any GPU supporting Vulkan.");
	}
	std::vector<VkPhysicalDevice> enumerateDevices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, enumerateDevices.data());
	std::vector<PhysicalDevice> devices;
	for (const VkPhysicalDevice device : enumerateDevices) {
		devices.push_back({ device });
	}
	for (PhysicalDevice device : devices) {
		if (device.isSuitable(surface)) {
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(device.device, &physicalDeviceProperties);
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
	NEIGE_ASSERT(physicalDevice.device != VK_NULL_HANDLE, "Unable to find a suitable GPU.");
	VkPhysicalDeviceProperties properties = physicalDevice.getProperties();
	NEIGE_INFO(properties.deviceName);

	// Queue family indices
	QueueFamilyIndices queueFamilyIndices = physicalDevice.findQueueFamilies(surface);
	std::set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphicsFamily.value(),
	queueFamilyIndices.computeFamily.value(),
	queueFamilyIndices.presentFamily.value() };
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.pNext = nullptr;
		deviceQueueCreateInfo.flags = 0;
		deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	// Physical device features
	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	physicalDeviceFeatures.sampleRateShading = VK_TRUE;

	// Logical device
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	if (NEIGE_DEBUG) {
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		deviceCreateInfo.ppEnabledLayerNames = layers.data();
	} else {
		deviceCreateInfo.enabledLayerCount = 0;
	}
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	NEIGE_VK_CHECK(vkCreateDevice(physicalDevice.device, &deviceCreateInfo, nullptr, &logicalDevice));

	// Queues
	vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &queues.graphicsQueue);
	vkGetDeviceQueue(logicalDevice, queueFamilyIndices.computeFamily.value(), 0, &queues.computeQueue);
	vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &queues.presentQueue);

	// Swapchain
	SwapchainSupport swapchainSupport = physicalDevice.swapchainSupport(surface);
	VkExtent2D extent = swapchainSupport.extent(window.getExtent());
	VkSurfaceFormatKHR surfaceFormat = swapchainSupport.surfaceFormat();
	VkPresentModeKHR presentMode = swapchainSupport.presentMode();
	uint32_t minImageCount = swapchainSupport.capabilities.minImageCount + 1;
	if (swapchainSupport.capabilities.maxImageCount > 0 && minImageCount > swapchainSupport.capabilities.maxImageCount) {
		minImageCount = swapchainSupport.capabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = minImageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (queueFamilyIndices.graphicsFamily.value() != queueFamilyIndices.presentFamily.value()) {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		std::array<uint32_t, 2> swapchainQueueFamilies = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value() };
		swapchainCreateInfo.pQueueFamilyIndices = swapchainQueueFamilies.data();
	} else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}
	swapchainCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	NEIGE_VK_CHECK(vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain));
}

void Renderer::destroy() {
	memoryAllocator.free(logicalDevice);
	if (NEIGE_DEBUG) {
		destroyDebugMessenger();
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
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

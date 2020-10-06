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
	std::vector<const char*> layers = {
			"VK_LAYER_KHRONOS_validation",
	};

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
}

void Renderer::destroy() {
	vkDestroyInstance(instance, nullptr);
}
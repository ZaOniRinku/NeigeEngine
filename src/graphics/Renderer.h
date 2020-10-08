#pragma once
#include <vulkan/vulkan.hpp>
#include "../utils/NeigeTools.h"
#include "../utils/NeigeStructs.h"
#include "../utils/MemoryAllocator.h"
#include "../window/Window.h"
#include "Instance.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include <iostream>
#include <vector>
#include <string>

struct Renderer {
	Window window;
	MemoryAllocator memoryAllocator;
	Instance instance;
	PhysicalDevice physicalDevice;
	LogicalDevice logicalDevice;
	Swapchain swapchain;

	void init();
	void destroy();
};
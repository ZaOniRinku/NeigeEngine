#pragma once
#include "vulkan/vulkan.hpp"
#include "PhysicalDevice.h"
#include "../../window/Window.h"

struct PhysicalDevicePicker {
	static void pick(Window* window);
};

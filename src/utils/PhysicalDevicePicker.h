#pragma once
#include "vulkan/vulkan.hpp"
#include "../graphics/PhysicalDevice.h"
#include "../window/Window.h"

struct PhysicalDevicePicker {
	static void pick(Window* window);
};

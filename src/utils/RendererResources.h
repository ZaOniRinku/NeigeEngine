#pragma once
#include "../graphics/Instance.h"
#include "../graphics/LogicalDevice.h"
#include "../graphics/PhysicalDevice.h"
#include "../graphics/Swapchain.h"
#include "../utils/MemoryAllocator.h"

extern Instance instance;
extern LogicalDevice logicalDevice;
extern PhysicalDevice physicalDevice;
extern Swapchain swapchain;
extern MemoryAllocator memoryAllocator;
#pragma once
#include "../graphics/Instance.h"
#include "../graphics/LogicalDevice.h"
#include "../graphics/PhysicalDevice.h"
#include "../graphics/Swapchain.h"
#include "../utils/MemoryAllocator.h"

inline Instance instance;
inline LogicalDevice logicalDevice;
inline PhysicalDevice physicalDevice;
inline Swapchain swapchain;
inline MemoryAllocator memoryAllocator;
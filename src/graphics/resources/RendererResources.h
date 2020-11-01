#pragma once
#include "../instance/Instance.h"
#include "../devices/LogicalDevice.h"
#include "../devices/PhysicalDevice.h"
#include "../renderpasses/Swapchain.h"
#include "../../utils/memoryallocator/MemoryAllocator.h"

inline Instance instance;
inline LogicalDevice logicalDevice;
inline PhysicalDevice physicalDevice;
inline Swapchain swapchain;
inline MemoryAllocator memoryAllocator;
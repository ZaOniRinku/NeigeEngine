#pragma once
#include "../instance/Instance.h"
#include "../devices/LogicalDevice.h"
#include "../devices/PhysicalDevice.h"
#include "../models/Model.h"
#include "../pipelines/Shader.h"
#include "../renderpasses/Swapchain.h"
#include "../../utils/memoryallocator/MemoryAllocator.h"

inline Instance instance;
inline LogicalDevice logicalDevice;
inline PhysicalDevice physicalDevice;
inline Swapchain swapchain;
inline MemoryAllocator memoryAllocator;
inline std::unordered_map<std::string, Model> models;
inline std::unordered_map<std::string, Shader> shaders;
#pragma once
#include "vulkan/vulkan.hpp"
#include "../../external/glslang/glslang/Include/ShHandle.h"
#include "../../external/glslang/SPIRV/GlslangToSpv.h"
#include "../../external/glslang/StandAlone/DirStackFileIncluder.h"
#include "../../external/spirv-reflect/spirv_reflect.h"
#include "../../utils/resources/FileTools.h"
#include "../../utils/NeigeDefines.h"
#include "../../utils/structs/RendererStructs.h"
#include "../../utils/structs/ShaderStructs.h"
#include <string>
#include <vector>

struct Shader {
	VkShaderModule module = VK_NULL_HANDLE;
	std::string file;
	ShaderType type;
	std::vector<uint32_t> spvCode;
	std::vector<Set> sets;
    std::vector<InputVariable> inputVariables;
	std::vector<ShaderType> layoutBindingsShaderTypes;
	std::vector<VkPushConstantRange> pushConstantRanges;
	std::set<VkDescriptorType> uniqueDescriptorTypes;
	bool glslInitialized = false;
    TBuiltInResource defaultTBuiltInResource = { 32,
        6,
        32,
        32,
        64,
        4096,
        64,
        32,
        80,
        32,
        4096,
        32,
        128,
        8,
        16,
        16,
        15,
        -8,
        7,
        8,
        65535,
        65535,
        65535,
        1024,
        1024,
        64,
        1024,
        16,
        8,
        8,
        1,
        60,
        64,
        64,
        128,
        128,
        8,
        8,
        8,
        0,
        0,
        0,
        0,
        0,
        8,
        8,
        16,
        256,
        1024,
        1024,
        64,
        128,
        128,
        16,
        1024,
        4096,
        128,
        128,
        16,
        1024,
        120,
        32,
        64,
        16,
        0,
        0,
        0,
        0,
        8,
        8,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        16384,
        4,
        64,
        8,
        8,
        4,
        256,
        512,
        32,
        1,
        1,
        32,
        1,
        1,
        4,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
    };

	void init(const std::string& filePath);
	void destroy();
	bool compile();
	void reflect();
	void reload();
	EShLanguage shaderTypeToGlslangShaderType();
	VkShaderStageFlagBits shaderTypeToVkShaderFlagBits();
};

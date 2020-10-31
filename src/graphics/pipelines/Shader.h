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
	std::vector<ShaderType> layoutBindingsShaderTypes;
	std::vector<VkPushConstantRange> pushConstantRanges;
	std::set<VkDescriptorType> uniqueDescriptorTypes;
	bool glslInitialized = false;

	void init(const std::string& filePath);
	void destroy();
	bool compile();
	void reflect();
	void reload();
	EShLanguage shaderTypeToGlslangShaderType();
	VkShaderStageFlagBits shaderTypeToVkShaderFlagBits();
};

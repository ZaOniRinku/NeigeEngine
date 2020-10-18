#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/FileTools.h"
#include "../utils/NeigeDefines.h"
#include "../../external/glslang/glslang/Include/ShHandle.h"
#include "../../external/glslang/SPIRV/GlslangToSpv.h"
#include "../../external/glslang/StandAlone/DirStackFileIncluder.h"
#include "../../external/spirv-reflect/spirv_reflect.h"
#include <string>
#include <vector>

enum SHADER_TYPE {
	VERTEX,
	FRAGMENT,
	TESSELATION_CONTROL,
	TESSELATION_EVALUATION,
	GEOMETRY,
	COMPUTE
};

struct Shader {
	VkShaderModule module;
	std::string file;
	enum SHADER_TYPE type;
	std::vector<uint32_t> spvCode;
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	std::vector<VkPushConstantRange> pushConstantRanges;
	bool glslInitialized = false;

	void init(const std::string& filePath);
	void destroy();
	void compile();
	void reflect();
	EShLanguage shaderTypeToGlslangShaderType();
	VkShaderStageFlagBits shaderTypeToVkShaderFlagBits();
};

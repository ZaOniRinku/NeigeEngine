#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/FileTools.h"
#include "../utils/NeigeDefines.h"
#include "../../external/glslang/glslang/Include/ShHandle.h"
#include "../../external/glslang/SPIRV/GlslangToSpv.h"
#include "../../external/glslang/StandAlone/DirStackFileIncluder.h"
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

struct Shader{
	VkShaderModule module;
	std::string file;
	SHADER_TYPE type;
	bool glslInitialized = false;
	void init(const std::string& filePath);
	void destroy();
	std::vector<uint32_t> compile(const std::string& filePath);
	EShLanguage shaderTypeToGlslangShaderType();
};

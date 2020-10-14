#include "Shader.h"
#include "../utils/RendererResources.h"

class glslang::TShader;

void Shader::init(const std::string& filePath) {
	file = filePath;
	std::string extension = FileTools::extension(filePath);
	if (extension == "vert") {
		type = VERTEX;
	}
	else if (extension == "frag") {
		type = FRAGMENT;
	}
	else if (extension == "tesc") {
		type = TESSELATION_CONTROL;
	}
	else if (extension == "tese") {
		type = TESSELATION_EVALUATION;
	}
	else if (extension == "geom") {
		type = GEOMETRY;
	}
	else if (extension == "comp") {
		type = COMPUTE;
	}
	else {
		NEIGE_ERROR("\"." + extension + "\" shader extension not supported.");
	}
	if (!glslInitialized) {
		glslang::InitializeProcess();
		glslInitialized = true;
	}
	compile(filePath);
}

void Shader::destroy() {
	vkDestroyShaderModule(logicalDevice.device, module, nullptr);
}

std::vector<uint32_t> Shader::compile(const std::string& filePath) {
	const TBuiltInResource DefaultTBuiltInResource = {};
	std::string code = FileTools::readAscii(filePath);
	const char* codeString = code.c_str();
	EShLanguage shaderType = shaderTypeToGlslangShaderType();
	glslang::TShader shader(shaderType);
	shader.setStrings(&codeString, 1);
	int clientInputSemanticsVersion = 120;
	glslang::EShTargetClientVersion vulkanClientVersion = glslang::EShTargetVulkan_1_2;
	glslang::EShTargetLanguageVersion languageVersion = glslang::EShTargetSpv_1_0;
	shader.setEnvInput(glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, clientInputSemanticsVersion);
	shader.setEnvClient(glslang::EShClientVulkan, vulkanClientVersion);
	shader.setEnvTarget(glslang::EShTargetSpv, languageVersion);
	TBuiltInResource resource = DefaultTBuiltInResource;
	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
	int defaultVersion = 450;

	// Preprocess
	DirStackFileIncluder includer;
	includer.pushExternalLocalDirectory(filePath);
	std::string preprocess;
	if (!shader.preprocess(&resource, defaultVersion, ENoProfile, false, false, messages, &preprocess, includer)) {
		NEIGE_ERROR("\"" + filePath + "\" shader preprocessing failed.\n" + "\"" + filePath + "\" error:" + shader.getInfoLog() + "\n" + "\"" + filePath + "\" error:" + shader.getInfoDebugLog());
	}

	// Parse
	const char* preprocessString = preprocess.c_str();
	shader.setStrings(&preprocessString, 1);
	if (!shader.parse(&resource, defaultVersion, false, messages)) {
		NEIGE_ERROR("\"" + filePath + "\" shader parcing failed.\n" + "\"" + filePath + "\" error:" + shader.getInfoLog() + "\n" + "\"" + filePath + "\" error:" + shader.getInfoDebugLog());
	}

	// Link
	glslang::TProgram program;
	program.addShader(&shader);
	if (!program.link(messages)) {
		NEIGE_ERROR("\"" + filePath + "\" shader linking failed.\n" + "\"" + filePath + "\" error:" + shader.getInfoLog() + "\n" + "\"" + filePath + "\" error:" + shader.getInfoDebugLog());
	}

	// Compile
	std::vector<uint32_t> spvCode;
	spv::SpvBuildLogger buildLogger;
	glslang::SpvOptions spvOptions;
	glslang::GlslangToSpv(*program.getIntermediate(shaderType), spvCode, &buildLogger, &spvOptions);
	
	return spvCode;
}

EShLanguage Shader::shaderTypeToGlslangShaderType() {
	switch (type) {
	case VERTEX:
		return EShLangVertex;
		break;
	case FRAGMENT:
		return EShLangFragment;
		break;
	case TESSELATION_CONTROL:
		return EShLangTessControl;
		break;
	case TESSELATION_EVALUATION:
		return EShLangTessEvaluation;
		break;
	case GEOMETRY:
		return EShLangGeometry;
		break;
	case COMPUTE:
		return EShLangCompute;
		break;
	default:
		NEIGE_ERROR("Error converting shader type to glslang shader type.");
	}
}

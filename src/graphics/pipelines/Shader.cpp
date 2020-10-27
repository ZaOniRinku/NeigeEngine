#include "Shader.h"
#include "../resources/RendererResources.h"

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

	// Compilation to SPIR-V
	compile();

	VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.flags = 0;
	shaderModuleCreateInfo.codeSize = spvCode.size() * sizeof(uint32_t);
	shaderModuleCreateInfo.pCode = spvCode.data();
	NEIGE_VK_CHECK(vkCreateShaderModule(logicalDevice.device, &shaderModuleCreateInfo, nullptr, &module));

	// Reflection from SPIR-V
	reflect();
}

void Shader::destroy() {
	layoutBindings.clear();
	layoutBindings.shrink_to_fit();
	layoutBindingsShaderTypes.clear();
	layoutBindingsShaderTypes.shrink_to_fit();
	vkDestroyShaderModule(logicalDevice.device, module, nullptr);
}

bool Shader::compile() {
	spvCode.clear();
	spvCode.shrink_to_fit();
	const TBuiltInResource DefaultTBuiltInResource = {};
	std::string code = FileTools::readAscii(file);
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
	resource.maxDrawBuffers = 32;
	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
	int defaultVersion = 450;

	// Preprocess
	DirStackFileIncluder includer;
	includer.pushExternalLocalDirectory(file);
	std::string preprocess;
	if (!shader.preprocess(&resource, defaultVersion, ENoProfile, false, false, messages, &preprocess, includer)) {
		NEIGE_SHADER_ERROR("\"" + file + "\" shader preprocessing failed.\n" + "\"" + shader.getInfoLog() + "\n" + shader.getInfoDebugLog());
		if (module != VK_NULL_HANDLE) {
			return false;
		}
		NEIGE_ERROR("Shader preprocess error.");
	}

	// Parse
	const char* preprocessString = preprocess.c_str();
	shader.setStrings(&preprocessString, 1);
	if (!shader.parse(&resource, defaultVersion, false, messages)) {
		NEIGE_SHADER_ERROR("\"" + file + "\" shader parsing failed.\n" + "\"" + shader.getInfoLog() + "\n" + shader.getInfoDebugLog());
		if (module != VK_NULL_HANDLE) {
			return false;
		}
		NEIGE_ERROR("Shader parsing error.");
	}

	// Link
	glslang::TProgram program;
	program.addShader(&shader);
	if (!program.link(messages)) {
		NEIGE_SHADER_ERROR("\"" + file + "\" shader linking failed.\n" + "\"" + shader.getInfoLog() + "\n" + shader.getInfoDebugLog());
		if (module != VK_NULL_HANDLE) {
			return false;
		}
		NEIGE_ERROR("Shader linking error.");
	}

	// Compile
	spv::SpvBuildLogger buildLogger;
	glslang::SpvOptions spvOptions;
	glslang::GlslangToSpv(*program.getIntermediate(shaderType), spvCode, &buildLogger, &spvOptions);

	return true;
}

void Shader::reflect() {
	pushConstantRanges.clear();
	pushConstantRanges.shrink_to_fit();
	uniqueDescriptorTypes.clear();

	SpvReflectShaderModule spvShaderModule;
	SpvReflectResult result = spvReflectCreateShaderModule(spvCode.size() * sizeof(uint32_t), spvCode.data(), &spvShaderModule);
	NEIGE_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "\"" + file + "\" shader reflection failed.");

	uint32_t descriptorSetCount;
	result = spvReflectEnumerateDescriptorSets(&spvShaderModule, &descriptorSetCount, nullptr);
	NEIGE_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "\"" + file + "\" : unable to count descriptors sets.");
	std::vector<SpvReflectDescriptorSet*> descriptorSets(descriptorSetCount);
	result = spvReflectEnumerateDescriptorSets(&spvShaderModule, &descriptorSetCount, descriptorSets.data());
	NEIGE_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "\"" + file + "\" : unable to find descriptors sets.");

	// Bindings
	for (uint32_t i = 0; i < descriptorSetCount; i++) {
		const SpvReflectDescriptorSet& reflectSet = *descriptorSets[i];
		for (uint32_t j = 0; j < reflectSet.binding_count; j++) {
			const SpvReflectDescriptorBinding& reflectBinding = *reflectSet.bindings[j];
			VkDescriptorSetLayoutBinding binding = {};
			binding.binding = reflectBinding.binding;
			binding.descriptorType = static_cast<VkDescriptorType>(reflectBinding.descriptor_type);
			uniqueDescriptorTypes.insert(binding.descriptorType);
			binding.descriptorCount = 1;
			for (uint32_t k = 0; k < reflectBinding.array.dims_count; k++) {
				binding.descriptorCount *= reflectBinding.array.dims[k];
			}
			binding.stageFlags = shaderTypeToVkShaderFlagBits();
			binding.pImmutableSamplers = nullptr;
			layoutBindings.push_back(binding);
			layoutBindingsShaderTypes.push_back(type);
		}
	}
	
	// Push constants
	uint32_t pushConstantCount;
	result = spvReflectEnumeratePushConstantBlocks(&spvShaderModule, &pushConstantCount, nullptr);
	NEIGE_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "\"" + file + "\" : unable to count push constants.");
	std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantCount);
	result = spvReflectEnumeratePushConstantBlocks(&spvShaderModule, &pushConstantCount, pushConstants.data());
	NEIGE_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "\"" + file + "\" : unable to find push constants.");
	
	for (uint32_t i = 0; i < pushConstantCount; i++) {
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.offset = pushConstants[i]->offset;
		pushConstantRange.size = pushConstants[i]->size;
		pushConstantRange.stageFlags = shaderTypeToVkShaderFlagBits();
		pushConstantRanges.push_back(pushConstantRange);
	}
	spvReflectDestroyShaderModule(&spvShaderModule);
}

void Shader::reload() {
	if (compile()) {
		destroy();

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = spvCode.size() * sizeof(uint32_t);
		shaderModuleCreateInfo.pCode = spvCode.data();
		NEIGE_VK_CHECK(vkCreateShaderModule(logicalDevice.device, &shaderModuleCreateInfo, nullptr, &module));

		reflect();
	}
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

VkShaderStageFlagBits Shader::shaderTypeToVkShaderFlagBits() {
	switch (type) {
	case VERTEX:
		return VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case FRAGMENT:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case TESSELATION_CONTROL:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		break;
	case TESSELATION_EVALUATION:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		break;
	case GEOMETRY:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case COMPUTE:
		return VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	default:
		NEIGE_ERROR("Error converting shader type to Vulkan shader flag bits.");
	}
}

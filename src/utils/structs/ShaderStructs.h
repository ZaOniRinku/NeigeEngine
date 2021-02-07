#pragma once
#include "vulkan/vulkan.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../external/glm/glm/glm.hpp"
#include "../NeigeDefines.h"

#define MAX_DIR_LIGHTS 10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10
#define MAX_BONES 256

enum struct ShaderType {
	VERTEX,
	FRAGMENT,
	TESSELATION_CONTROL,
	TESSELATION_EVALUATION,
	GEOMETRY,
	COMPUTE
};

enum struct Topology {
	TRIANGLE_LIST,
	TRIANGLE_STRIP,
	LINE_STRIP,
	POINT_LIST,
	WIREFRAME
};

enum struct LightType {
	DIRECTIONAL,
	POINT,
	SPOT
};

// Material
struct Material {
	std::string diffuseKey = "";
	std::string normalKey = "";
	std::string metallicRoughnessKey = "";
	std::string emissiveKey = "";
	std::string occlusionKey = "";
};

// Input variable
struct InputVariable {
	uint32_t location;
	std::string name;
};

// Binding
struct Binding {
	VkDescriptorSetLayoutBinding binding;
	std::string name;
};

// Set
struct Set {
	uint32_t set;
	std::vector<Binding> bindings;
};

// Vertex
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;
	glm::vec3 tangent;
	glm::vec4 joints;
	glm::vec4 weights;

	static VkVertexInputBindingDescription getInputBindingDescription() {
		VkVertexInputBindingDescription inputBindingDescription = {};
		inputBindingDescription.binding = 0;
		inputBindingDescription.stride = sizeof(Vertex);
		inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return inputBindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions(std::vector<InputVariable> inputVariables) {
		std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions;

		for (size_t i = 0; i < inputVariables.size(); i++) {
			if (inputVariables[i].name == "position") {
				VkVertexInputAttributeDescription positionAttribute = {};
				positionAttribute.binding = 0;
				positionAttribute.location = inputVariables[i].location;
				positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				positionAttribute.offset = offsetof(Vertex, position);
				inputAttributeDescriptions.push_back(positionAttribute);
			}
			else if (inputVariables[i].name == "normal") {
				VkVertexInputAttributeDescription normalAttribute = {};
				normalAttribute.binding = 0;
				normalAttribute.location = inputVariables[i].location;
				normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				normalAttribute.offset = offsetof(Vertex, normal);
				inputAttributeDescriptions.push_back(normalAttribute);
			}
			else if (inputVariables[i].name == "uv") {
				VkVertexInputAttributeDescription uvAttribute = {};
				uvAttribute.binding = 0;
				uvAttribute.location = inputVariables[i].location;
				uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
				uvAttribute.offset = offsetof(Vertex, uv);
				inputAttributeDescriptions.push_back(uvAttribute);
			}
			else if (inputVariables[i].name == "color") {
				VkVertexInputAttributeDescription colorAttribute = {};
				colorAttribute.binding = 0;
				colorAttribute.location = inputVariables[i].location;
				colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				colorAttribute.offset = offsetof(Vertex, color);
				inputAttributeDescriptions.push_back(colorAttribute);
			}
			else if (inputVariables[i].name == "tangent") {
				VkVertexInputAttributeDescription tangentAttribute = {};
				tangentAttribute.binding = 0;
				tangentAttribute.location = inputVariables[i].location;
				tangentAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				tangentAttribute.offset = offsetof(Vertex, tangent);
				inputAttributeDescriptions.push_back(tangentAttribute);
			}
			else if (inputVariables[i].name == "joints") {
				VkVertexInputAttributeDescription jointsAttribute = {};
				jointsAttribute.binding = 0;
				jointsAttribute.location = inputVariables[i].location;
				jointsAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				jointsAttribute.offset = offsetof(Vertex, joints);
				inputAttributeDescriptions.push_back(jointsAttribute);
			}
			else if (inputVariables[i].name == "weights") {
				VkVertexInputAttributeDescription weightsAttribute = {};
				weightsAttribute.binding = 0;
				weightsAttribute.location = inputVariables[i].location;
				weightsAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				weightsAttribute.offset = offsetof(Vertex, weights);
				inputAttributeDescriptions.push_back(weightsAttribute);
			}
			else {
				NEIGE_WARNING("Vertex shader input variable \"" + inputVariables[i].name + "\" at location " + std::to_string(inputVariables[i].location) + " is undefined.");
			}
		}

		return inputAttributeDescriptions;
	}
};

// Object Uniform Buffer Object
struct ObjectUniformBufferObject {
	glm::mat4 model;
};

// Camera Uniform Buffer Object
struct CameraUniformBufferObject {
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 position;
};

// Lights Uniform Buffer Object
struct LightingUniformBufferObject {
	glm::vec4 numLights;
	glm::vec4 dirLightsDirection[MAX_DIR_LIGHTS];
	glm::vec4 dirLightsColor[MAX_DIR_LIGHTS];
	glm::vec4 pointLightsPosition[MAX_POINT_LIGHTS];
	glm::vec4 pointLightsColor[MAX_POINT_LIGHTS];
	glm::vec4 spotLightsPosition[MAX_SPOT_LIGHTS];
	glm::vec4 spotLightsDirection[MAX_SPOT_LIGHTS];
	glm::vec4 spotLightsColor[MAX_SPOT_LIGHTS];
	glm::vec4 spotLightsCutoffs[MAX_SPOT_LIGHTS];
};

// Shadow Uniform Buffer Object
struct ShadowUniformBufferObject {
	glm::vec4 numLights;
	glm::mat4 dirLightSpaces[MAX_DIR_LIGHTS];
	glm::mat4 spotLightSpaces[MAX_SPOT_LIGHTS];
};

// Bone Uniform Buffer Object
struct BoneUniformBufferObject {
	glm::mat4 transformations[MAX_BONES];
	glm::mat4 inverseBindMatrices[MAX_BONES];
};
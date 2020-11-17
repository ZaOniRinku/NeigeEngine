#pragma once
#include "vulkan/vulkan.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../external/glm/glm/glm.hpp"
#include "../NeigeDefines.h"

#define MAX_DIR_LIGHTS 10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

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
	std::string occlusionKey = "";
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

	static std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions(std::vector<std::string> inputVariablesNames, std::vector<uint32_t> inputVariablesIndices) {
		std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions;

		for (size_t i = 0; i < inputVariablesNames.size(); i++) {
			if (inputVariablesNames[i] == "position") {
				VkVertexInputAttributeDescription positionAttribute = {};
				positionAttribute.binding = 0;
				positionAttribute.location = inputVariablesIndices[i];
				positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				positionAttribute.offset = offsetof(Vertex, position);
				inputAttributeDescriptions.push_back(positionAttribute);
			}
			else if (inputVariablesNames[i] == "normal") {
				VkVertexInputAttributeDescription normalAttribute = {};
				normalAttribute.binding = 0;
				normalAttribute.location = inputVariablesIndices[i];
				normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				normalAttribute.offset = offsetof(Vertex, normal);
				inputAttributeDescriptions.push_back(normalAttribute);
			}
			else if (inputVariablesNames[i] == "uv") {
				VkVertexInputAttributeDescription uvAttribute = {};
				uvAttribute.binding = 0;
				uvAttribute.location = inputVariablesIndices[i];
				uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
				uvAttribute.offset = offsetof(Vertex, uv);
				inputAttributeDescriptions.push_back(uvAttribute);
			}
			else if (inputVariablesNames[i] == "color") {
				VkVertexInputAttributeDescription colorAttribute = {};
				colorAttribute.binding = 0;
				colorAttribute.location = inputVariablesIndices[i];
				colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				colorAttribute.offset = offsetof(Vertex, color);
				inputAttributeDescriptions.push_back(colorAttribute);
			}
			else if (inputVariablesNames[i] == "tangent") {
				VkVertexInputAttributeDescription tangentAttribute = {};
				tangentAttribute.binding = 0;
				tangentAttribute.location = inputVariablesIndices[i];
				tangentAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				tangentAttribute.offset = offsetof(Vertex, tangent);
				inputAttributeDescriptions.push_back(tangentAttribute);
			}
			else if (inputVariablesNames[i] == "joints") {
				VkVertexInputAttributeDescription jointsAttribute = {};
				jointsAttribute.binding = 0;
				jointsAttribute.location = inputVariablesIndices[i];
				jointsAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				jointsAttribute.offset = offsetof(Vertex, joints);
				inputAttributeDescriptions.push_back(jointsAttribute);
			}
			else if (inputVariablesNames[i] == "weights") {
				VkVertexInputAttributeDescription weightsAttribute = {};
				weightsAttribute.binding = 0;
				weightsAttribute.location = inputVariablesIndices[i];
				weightsAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				weightsAttribute.offset = offsetof(Vertex, weights);
				inputAttributeDescriptions.push_back(weightsAttribute);
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
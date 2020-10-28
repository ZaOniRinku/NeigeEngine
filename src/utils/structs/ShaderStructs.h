#pragma once
#include "vulkan/vulkan.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../external/glm/glm/glm.hpp"
#include "../NeigeDefines.h"

enum struct ShaderType {
	VERTEX,
	FRAGMENT,
	TESSELATION_CONTROL,
	TESSELATION_EVALUATION,
	GEOMETRY,
	COMPUTE
};

// Vertex
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;
	glm::vec3 tangent;

	static VkVertexInputBindingDescription getInputBindingDescription() {
		VkVertexInputBindingDescription inputBindingDescription = {};
		inputBindingDescription.binding = 0;
		inputBindingDescription.stride = sizeof(Vertex);
		inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return inputBindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions;
		VkVertexInputAttributeDescription positionAttribute = {};
		positionAttribute.binding = 0;
		positionAttribute.location = 0;
		positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionAttribute.offset = offsetof(Vertex, position);
		inputAttributeDescriptions.push_back(positionAttribute);

		VkVertexInputAttributeDescription normalAttribute = {};
		normalAttribute.binding = 0;
		normalAttribute.location = 1;
		normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalAttribute.offset = offsetof(Vertex, normal);
		inputAttributeDescriptions.push_back(normalAttribute);

		VkVertexInputAttributeDescription uvAttribute = {};
		uvAttribute.binding = 0;
		uvAttribute.location = 2;
		uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		uvAttribute.offset = offsetof(Vertex, uv);
		inputAttributeDescriptions.push_back(uvAttribute);

		VkVertexInputAttributeDescription colorAttribute = {};
		colorAttribute.binding = 0;
		colorAttribute.location = 3;
		colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		colorAttribute.offset = offsetof(Vertex, color);
		inputAttributeDescriptions.push_back(colorAttribute);

		VkVertexInputAttributeDescription tangentAttribute = {};
		tangentAttribute.binding = 0;
		tangentAttribute.location = 4;
		tangentAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		tangentAttribute.offset = offsetof(Vertex, tangent);
		inputAttributeDescriptions.push_back(tangentAttribute);

		return inputAttributeDescriptions;
	}
};

// Object Uniform Buffer Object
struct ObjectUniformBufferObject {
	glm::mat4 model;
};

// Camera Uniform Buffer Object
struct CameraUniformBufferObject {
	glm::mat4 viewProj;
	glm::vec3 position;
};
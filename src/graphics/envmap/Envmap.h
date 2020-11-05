#pragma once
#include "vulkan/vulkan.hpp"
#include "../../ecs/components/Camera.h"
#include "../../utils/structs/ShaderStructs.h"
#include "../../utils/structs/RendererStructs.h"
#include "../commands/CommandPool.h"
#include "../commands/CommandBuffer.h"
#include "../pipelines/Viewport.h"
#include "../pipelines/DescriptorSet.h"
#include "../pipelines/GraphicsPipeline.h"
#include "../resources/Buffer.h"
#include "../resources/Image.h"
#include "../renderpasses/Framebuffer.h"
#include "../renderpasses/RenderPass.h"
#include "../../utils/resources/BufferTools.h"
#include "../../utils/resources/ImageTools.h"
#include <numeric>

struct Envmap {
	// Unit cube
	Buffer cubeVertexBuffer;
	Buffer cubeIndexBuffer;

	// Equilateral rectangle to Cubemap
	Viewport equiRecToCubemapViewport;
	GraphicsPipeline equiRecToCubemapGraphicsPipeline;
	RenderPass equiRecToCubemapRenderPass;
	std::array<Framebuffer, 6> equiRecToCubemapFramebuffers;
	DescriptorSet equiRecToCubemapDescriptorSet;
	std::array<VkImageView, 6> equiRecToCubemapImageViews;

	// Display skybox
	GraphicsPipeline skyboxGraphicsPipeline;

	Image defaultSkybox;
	Image envmapImage;
	Image skyboxImage;

	std::vector<Vertex> cubeVertices = { Vertex { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },

		Vertex { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },

		Vertex { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },

		Vertex { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },

		Vertex { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },

		Vertex { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) } };

	void init(std::string filePath);
	void destroy();
	void draw(CommandBuffer* commandBuffer);
	void equilateralRectangleToCubemap();
};
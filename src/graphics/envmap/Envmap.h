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

#define CONVOLVE_WIDTH 32
#define CONVOLVE_HEIGHT 32

#define PREFILTER_WIDTH 128
#define PREFILTER_HEIGHT 128

#define BRDFCONVOLUTION_WIDTH 512
#define BRDFCONVOLUTION_HEIGHT 512

struct Envmap {
	// Unit cube
	Buffer cubeVertexBuffer;
	Buffer cubeIndexBuffer;

	// NDC screen space quad
	Buffer quadVertexBuffer;
	Buffer quadIndexBuffer;

	Image defaultSkybox;
	Image envmapImage;
	Image skyboxImage;
	Image diffuseIradianceImage;
	Image prefilterImage;
	Image brdfConvolutionImage;

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
	std::vector<Vertex> quadVertices = { Vertex { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(1.0f,  -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
		Vertex { glm::vec3(-1.0f,  -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f) } };

	void init(std::string filePath);
	void destroy();
	void draw(CommandBuffer* commandBuffer);
	void equilateralRectangleToCubemap();
	void createDiffuseIradiance();
	void createPrefilter();
	void createBRDFConvolution();
};
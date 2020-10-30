#pragma once
#include "../../utils/structs/ShaderStructs.h"
#include <string>

struct Renderable {
	// Model
	std::string modelPath = "";

	// Shaders
	std::string vertexShaderPath = "";
	std::string fragmentShaderPath = "";
	std::string tesselationControlShaderPath = "";
	std::string tesselationEvaluationShaderPath = "";
	std::string geometryShaderPath = "";

	// Pipeline topology
	Topology topology = Topology::TRIANGLE_LIST;
};
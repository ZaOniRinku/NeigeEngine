#pragma once
#include <string>

struct Renderable {
	std::string vertexShaderPath = "";
	std::string fragmentShaderPath = "";
	std::string tesselationControlShaderPath = "";
	std::string tesselationEvaluationShaderPath = "";
	std::string geometryShaderPath = "";
};
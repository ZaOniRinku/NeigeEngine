#pragma once
#pragma warning(disable:4996)
#include "../../graphics/resources/Buffer.h"
#include "../NeigeDefines.h"
#include "../structs/ShaderStructs.h"
#include "BufferTools.h"
#include "FileTools.h"

struct ModelLoader {
	static void load(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Primitive>* primitives);
	static void loadglTF(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Primitive>* primitives);
};
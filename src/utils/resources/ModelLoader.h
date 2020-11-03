#pragma once
#pragma warning(disable:4996)
#include "../../graphics/resources/Image.h"
#include "../NeigeDefines.h"
#include "../structs/ModelStructs.h"
#include "../structs/ShaderStructs.h"
#include "../../../external/glm/glm/gtc/type_ptr.hpp"
#include "../../../external/glm/glm/gtx/quaternion.hpp"
#include "FileTools.h"
#include "ImageTools.h"

struct ModelLoader {
	static void load(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes);
	static void loadglTF(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes);
};
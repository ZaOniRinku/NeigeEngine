#pragma once
#pragma warning(disable:4996)
#include "../../graphics/resources/Image.h"
#include "../NeigeDefines.h"
#include "../structs/ModelStructs.h"
#include "../structs/ShaderStructs.h"
#include "../../../external/glm/glm/gtc/type_ptr.hpp"
#include "../../../external/glm/glm/gtx/quaternion.hpp"
#include "../../external/cgltf/cgltf.h"
#include "FileTools.h"
#include "ImageTools.h"
#include <vector>
#include <numeric>

struct ModelLoader {
	static void load(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes);
	static void loadglTF(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes);
	static void loadglTFNode(const std::string& filePath, cgltf_node* node, uint32_t* indexOffset, int32_t* modelVertexOffset, glm::mat4 modelMatrix, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes);
};
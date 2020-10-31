#include "ModelLoader.h"
#define CGLTF_IMPLEMENTATION
#include "../../external/cgltf/cgltf.h"

void ModelLoader::load(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Primitive>* primitives) {
	std::string extension = FileTools::extension(filePath);
	if (extension == "gltf" || extension == "glb") {
		loadglTF(filePath, vertices, indices, primitives);
	}
	else {
		NEIGE_ERROR("\"." + extension + "\" model extension not supported.");
	}
}

void ModelLoader::loadglTF(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Primitive>* primitives) {
	cgltf_options options = {};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, filePath.c_str(), &data);
	if (result == cgltf_result_success) {
		result = cgltf_load_buffers(&options, data, filePath.c_str());
		if (result != cgltf_result_success) {
			NEIGE_ERROR("Cannot load buffers for model file \"" + filePath + "\".");
		}

		uint32_t firstIndex = 0;
		uint32_t indexCount = 0;
		if (result == cgltf_result_success) {
			for (size_t i = 0; i < data->nodes_count; i++) {
				if (data->nodes[i].mesh != NULL) {
					cgltf_mesh* mesh = data->nodes[i].mesh;

					for (size_t j = 0; j < mesh->primitives_count; j++) {
						cgltf_primitive* primitive = &mesh->primitives[j];

						firstIndex += indexCount;
						indexCount = 0;

						float* position;
						float* normal;
						float* uv;
						float* color;
						float* tangent;

						size_t positionCount = 0;
						size_t normalCount = 0;
						size_t uvCount = 0;
						size_t colorCount = 0;
						size_t tangentCount = 0;

						for (size_t k = 0; k < primitive->attributes_count; k++) {
							cgltf_attribute* attribute = &primitive->attributes[k];
							std::string attributeName = attribute->name;
							if (attributeName == "POSITION") {
								cgltf_accessor* accessor = attribute->data;
								cgltf_buffer_view* buffer_view = accessor->buffer_view;
								char* buffer = static_cast<char *>(buffer_view->buffer->data);
								position = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
								positionCount = attribute->data->count;
							}
							else if (attributeName == "NORMAL") {
								cgltf_accessor* accessor = attribute->data;
								cgltf_buffer_view* buffer_view = accessor->buffer_view;
								char* buffer = static_cast<char*>(buffer_view->buffer->data);
								normal = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
								normalCount = attribute->data->count;
							}
							else if (attributeName == "TEXCOORD_0") {
								cgltf_accessor* accessor = attribute->data;
								cgltf_buffer_view* buffer_view = accessor->buffer_view;
								char* buffer = static_cast<char*>(buffer_view->buffer->data);
								uv = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
								uvCount = attribute->data->count;
							}
							else if (attributeName == "COLOR_0") {
								cgltf_accessor* accessor = attribute->data;
								cgltf_buffer_view* buffer_view = accessor->buffer_view;
								char* buffer = static_cast<char*>(buffer_view->buffer->data);
								color = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
								colorCount = attribute->data->count;
							}
							else if (attributeName == "TANGENT") {
								cgltf_accessor* accessor = attribute->data;
								cgltf_buffer_view* buffer_view = accessor->buffer_view;
								char* buffer = static_cast<char*>(buffer_view->buffer->data);
								tangent = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
								tangentCount = attribute->data->count;
							}
						}

						// Vertices
						int uvPos = 0;
						for (size_t l = 0; l < positionCount * 3; l += 3) {
							Vertex vertex = {};
							vertex.position = glm::vec3(position[l + 0], position[l + 1], position[l + 2]);
							vertex.normal = normalCount != 0 ? glm::vec3(normal[l + 0], normal[l + 1], normal[l + 2]) : glm::vec3(0.0f);
							vertex.uv = uvCount != 0 ? glm::vec2(uv[uvPos + 0], uv[uvPos + 1]) : glm::vec2(0.5f);
							vertex.color = colorCount != 0 ? glm::vec3(color[l + 0], color[l + 1], color[l + 2]) : glm::vec3(0.5f);
							vertex.tangent = tangentCount != 0 ? glm::vec3(tangent[l + 0], tangent[l + 1], tangent[l + 2]) : glm::vec3(0.0f);
							vertices->push_back(vertex);
							uvPos += 2;
						}

						// Indices
						cgltf_accessor* accessor = primitive->indices;
						cgltf_buffer_view* buffer_view = accessor->buffer_view;
						cgltf_component_type component_type = accessor->component_type;
						char* buffer = static_cast<char*>(buffer_view->buffer->data);
						switch (component_type) {
						case cgltf_component_type_r_8:
						{
							int8_t* index = reinterpret_cast<int8_t*>(buffer + accessor->offset + buffer_view->offset);
							for (size_t l = 0; l < accessor->count; l++) {
								indices->push_back(static_cast<uint32_t>(index[l]));
							}
							break;
						}
						case cgltf_component_type_r_8u:
						{
							uint8_t* index = reinterpret_cast<uint8_t*>(buffer + accessor->offset + buffer_view->offset);
							for (size_t l = 0; l < accessor->count; l++) {
								indices->push_back(static_cast<uint32_t>(index[l]));
							}
							break;
						}
						case cgltf_component_type_r_16:
						{
							int16_t* index = reinterpret_cast<int16_t*>(buffer + accessor->offset + buffer_view->offset);
							for (size_t l = 0; l < accessor->count; l++) {
								indices->push_back(static_cast<uint32_t>(index[l]));
							}
							break;
						}
						case cgltf_component_type_r_16u:
						{
							uint16_t* index = reinterpret_cast<uint16_t*>(buffer + accessor->offset + buffer_view->offset);
							for (size_t l = 0; l < accessor->count; l++) {
								indices->push_back(static_cast<uint32_t>(index[l]));
							}
							break;
						}
						case cgltf_component_type_r_32u:
						{
							uint32_t* index = reinterpret_cast<uint32_t*>(buffer + accessor->offset + buffer_view->offset);
							for (size_t l = 0; l < accessor->count; l++) {
								indices->push_back(static_cast<uint32_t>(index[l]));
							}
							break;
						}
						case cgltf_component_type_r_32f:
						{
							float* index = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
							for (size_t l = 0; l < accessor->count; l++) {
								indices->push_back(static_cast<uint32_t>(index[l]));
							}
							break;
						}
						default:
							NEIGE_ERROR("Indices component type unknown for model file \"" + filePath + "\".");
						}

						// Primitive
						indexCount = static_cast<uint32_t>(accessor->count);
						primitives->push_back({ firstIndex, indexCount });
					}
				}
			}
		}
		cgltf_free(data);
	}
	else if (result == cgltf_result_file_not_found) {
		NEIGE_ERROR("Model file \"" + filePath + "\" could not be opened.");
	}
	else {
		NEIGE_ERROR("Error with model file \"" + filePath + "\".");
	}
}
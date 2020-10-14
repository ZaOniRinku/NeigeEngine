#include "ModelLoader.h"
#define CGLTF_IMPLEMENTATION
#include "../../external/cgltf/cgltf.h"

void ModelLoader::load(const std::string& filePath) {
	std::string extension = FileTools::extension(filePath);
	if (extension == "gltf" || extension == "glb") {
		loadglTF(filePath);
	}
	else {
		NEIGE_ERROR("\"." + extension + "\" model extension not supported.");
	}
}

void ModelLoader::loadglTF(const std::string& filePath) {
	cgltf_options options = {};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, filePath.c_str(), &data);
	if (result == cgltf_result_success) {
		for (int i = 0; i < data->nodes_count; i++) {
			if (data->nodes[i].mesh != NULL) {
				for (int j = 0; j < data->nodes[i].mesh->primitives_count; j++) {
					for (int k = 0; k < data->nodes[i].mesh->primitives[j].attributes_count; k++) {
						std::cout << data->nodes[i].mesh->primitives[j].attributes[k].name << std::endl;
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
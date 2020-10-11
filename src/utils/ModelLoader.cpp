#include "ModelLoader.h"
#define CGLTF_IMPLEMENTATION
#include "../../external/cgltf/cgltf.h"

void ModelLoader::load(std::string filePath) {
	std::string extension = FileTools::extension(filePath);
	if (extension == "gltf" || extension == "glb") {
		loadglTF(filePath);
	}
	else {
		NEIGE_ERROR("." + filePath + " model extension not supported.");
	}
}

void ModelLoader::loadglTF(std::string filePath) {
	cgltf_options options = {};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, filePath.c_str(), &data);
	if (result == cgltf_result_success) {
		result = cgltf_load_buffers(&options, data, filePath.c_str());
		if (result == cgltf_result_success) {
			for (size_t i = 0; i < data->buffers_count; i++) {
				std::cout << data->buffers[i].size << std::endl;
			}
		}
		else {
			NEIGE_ERROR("Error with buffer loading for file \"" + filePath + "\".");
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
#include "ModelLoader.h"
#define CGLTF_IMPLEMENTATION
#include "../../external/cgltf/cgltf.h"
#include "../../graphics/resources/RendererResources.h"
#include "../../graphics/resources/ShaderResources.h"

void ModelLoader::load(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes) {
	std::string extension = FileTools::extension(filePath);
	if (extension == "gltf" || extension == "glb") {
		loadglTF(filePath, vertices, indices, meshes);
	}
	else {
		NEIGE_ERROR("\"." + extension + "\" model extension not supported.");
	}
}

void ModelLoader::loadglTF(const std::string& filePath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes) {
	cgltf_options options = {};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, filePath.c_str(), &data);
	if (result == cgltf_result_success) {
		result = cgltf_load_buffers(&options, data, filePath.c_str());

		if (result != cgltf_result_success) {
			NEIGE_ERROR("Cannot load buffers for model file \"" + filePath + "\".");
		}

		if (result == cgltf_result_success) {
			cgltf_scene* scene = data->scene;

			uint32_t indexOffset = 0;
			int32_t modelVertexOffset = 0;

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			
			for (size_t i = 0; i < scene->nodes_count; i++) {
				cgltf_node* node = scene->nodes[i];

				loadglTFNode(filePath, node, &indexOffset, &modelVertexOffset, modelMatrix, vertices, indices, meshes);
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

void ModelLoader::loadglTFNode(const std::string& filePath, cgltf_node* node, uint32_t* indexOffset, int32_t* modelVertexOffset, glm::mat4 modelMatrix, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, std::vector<Mesh>* meshes) {
	if (node->has_matrix) {
		cgltf_float* matrix = node->matrix;
		modelMatrix = modelMatrix * glm::make_mat4(matrix);
	}
	else {
		if (node->has_translation) {
			cgltf_float* translation = node->translation;
			glm::mat4 meshTranslation = glm::mat4(1.0f);
			meshTranslation[3][0] = translation[0];
			meshTranslation[3][1] = translation[1];
			meshTranslation[3][2] = translation[2];
			modelMatrix = modelMatrix * meshTranslation;
		}

		if (node->has_rotation) {
			cgltf_float* rotation = node->rotation;
			glm::quat rotationQuaternion = glm::quat(rotation[0], rotation[1], rotation[2], rotation[3]);
			glm::mat4 meshRotation = glm::toMat4(rotationQuaternion);
			modelMatrix = modelMatrix * meshRotation;
		}

		if (node->has_scale) {
			cgltf_float* scale = node->scale;
			glm::mat4 meshScale = glm::mat4(1.0f);
			meshScale[0][0] = scale[0];
			meshScale[1][1] = scale[1];
			meshScale[2][2] = scale[2];
			modelMatrix = modelMatrix * meshScale;
		}
	}
	
	if (node->mesh != NULL) {
		cgltf_mesh* mesh = node->mesh;

		Mesh modelMesh;

		modelMesh.indexOffset = *indexOffset;
		modelMesh.vertexOffset = *modelVertexOffset;

		std::vector<Primitive> primitives;

		uint32_t firstIndex = 0;
		uint32_t indexCount = 0;
		int32_t vertexOffset = 0;
		int32_t vertexCount = 0;

		for (size_t j = 0; j < mesh->primitives_count; j++) {
			cgltf_primitive* primitive = &mesh->primitives[j];

			std::vector<Vertex> primitiveVertices;
			std::vector<uint32_t> primitiveIndices;

			firstIndex += indexCount;
			indexCount = 0;
			vertexOffset += vertexCount;
			vertexCount = 0;

			float* position;
			float* normal;
			float* uv;
			float* color;
			float* tangent;
			unsigned short* joints;
			float* weights;

			size_t positionCount = 0;
			size_t normalCount = 0;
			size_t uvCount = 0;
			size_t colorCount = 0;
			size_t tangentCount = 0;
			size_t jointsCount = 0;
			size_t weightsCount = 0;

			for (size_t k = 0; k < primitive->attributes_count; k++) {
				cgltf_attribute* attribute = &primitive->attributes[k];
				std::string attributeName = attribute->name;

				cgltf_accessor* accessor = attribute->data;
				cgltf_buffer_view* buffer_view = accessor->buffer_view;
				char* buffer = static_cast<char*>(buffer_view->buffer->data);
				if (attributeName == "POSITION") {
					position = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
					positionCount = attribute->data->count;
				}
				else if (attributeName == "NORMAL") {
					normal = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
					normalCount = attribute->data->count;
				}
				else if (attributeName == "TEXCOORD_0") {
					uv = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
					uvCount = attribute->data->count;
				}
				else if (attributeName == "COLOR_0") {
					color = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
					colorCount = attribute->data->count;
				}
				else if (attributeName == "TANGENT") {
					tangent = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
					tangentCount = attribute->data->count;
				}
				else if (attributeName == "JOINTS_0") {
					joints = reinterpret_cast<unsigned short*>(buffer + accessor->offset + buffer_view->offset);
					jointsCount = attribute->data->count;
				}
				else if (attributeName == "WEIGHTS_0") {
					weights = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
					weightsCount = attribute->data->count;
				}
			}

			// Vertices
			int uvPos = 0;
			int jointWeightPos = 0;
			for (size_t l = 0; l < positionCount * 3; l += 3) {
				Vertex vertex = {};
				vertex.position = glm::vec3(position[l + 0], position[l + 1], position[l + 2]);
				vertex.position = glm::vec3(modelMatrix * glm::vec4(vertex.position, 1.0f));
				vertex.normal = normalCount != 0 ? glm::vec3(normal[l + 0], normal[l + 1], normal[l + 2]) : glm::vec3(0.0f);
				vertex.normal = glm::normalize(glm::transpose(glm::inverse(glm::mat3(modelMatrix))) * vertex.normal);
				vertex.uv = uvCount != 0 ? glm::vec2(uv[uvPos + 0], uv[uvPos + 1]) : glm::vec2(0.5f);
				vertex.color = colorCount != 0 ? glm::vec3(color[l + 0], color[l + 1], color[l + 2]) : glm::vec3(0.5f);
				vertex.tangent = tangentCount != 0 ? glm::vec3(tangent[l + 0], tangent[l + 1], tangent[l + 2]) : glm::vec3(0.0f);
				vertex.joints = jointsCount != 0 ? glm::vec4(joints[jointWeightPos + 0], joints[jointWeightPos + 1], joints[jointWeightPos + 2], joints[jointWeightPos + 3]) : glm::vec4(0.0f);
				vertex.weights = weightsCount != 0 ? glm::vec4(weights[jointWeightPos + 0], weights[jointWeightPos + 1], weights[jointWeightPos + 2], weights[jointWeightPos + 3]) : glm::vec4(0.0f);
				primitiveVertices.push_back(vertex);
				uvPos += 2;
				jointWeightPos += 4;
			}
			vertexCount += static_cast<int32_t>(primitiveVertices.size());

			// Indices
			cgltf_accessor* accessor = primitive->indices;
			if (accessor != NULL) {
				cgltf_buffer_view* buffer_view = accessor->buffer_view;
				cgltf_component_type component_type = accessor->component_type;
				char* buffer = static_cast<char*>(buffer_view->buffer->data);
				switch (component_type) {
				case cgltf_component_type_r_8:
				{
					int8_t* index = reinterpret_cast<int8_t*>(buffer + accessor->offset + buffer_view->offset);
					for (size_t l = 0; l < accessor->count; l++) {
						primitiveIndices.push_back(static_cast<uint32_t>(index[l]));
					}
					break;
				}
				case cgltf_component_type_r_8u:
				{
					uint8_t* index = reinterpret_cast<uint8_t*>(buffer + accessor->offset + buffer_view->offset);
					for (size_t l = 0; l < accessor->count; l++) {
						primitiveIndices.push_back(static_cast<uint32_t>(index[l]));
					}
					break;
				}
				case cgltf_component_type_r_16:
				{
					int16_t* index = reinterpret_cast<int16_t*>(buffer + accessor->offset + buffer_view->offset);
					for (size_t l = 0; l < accessor->count; l++) {
						primitiveIndices.push_back(static_cast<uint32_t>(index[l]));
					}
					break;
				}
				case cgltf_component_type_r_16u:
				{
					uint16_t* index = reinterpret_cast<uint16_t*>(buffer + accessor->offset + buffer_view->offset);
					for (size_t l = 0; l < accessor->count; l++) {
						primitiveIndices.push_back(static_cast<uint32_t>(index[l]));
					}
					break;
				}
				case cgltf_component_type_r_32u:
				{
					uint32_t* index = reinterpret_cast<uint32_t*>(buffer + accessor->offset + buffer_view->offset);
					for (size_t l = 0; l < accessor->count; l++) {
						primitiveIndices.push_back(static_cast<uint32_t>(index[l]));
					}
					break;
				}
				case cgltf_component_type_r_32f:
				{
					float* index = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);
					for (size_t l = 0; l < accessor->count; l++) {
						primitiveIndices.push_back(static_cast<uint32_t>(index[l]));
					}
					break;
				}
				default:
					NEIGE_ERROR("Indices component type unknown for model file \"" + filePath + "\".");
				}
				indexCount = static_cast<uint32_t>(accessor->count);
			}
			else {
				primitiveIndices.resize(primitiveVertices.size());
				std::iota(primitiveIndices.begin(), primitiveIndices.end(), 0);
				indexCount = static_cast<uint32_t>(primitiveIndices.size());
			}

			// Tangents
			if (tangentCount == 0) {
				for (size_t l = 0; l < primitiveIndices.size(); l += 3) {
					Vertex* vertex0 = &primitiveVertices.at(primitiveIndices.at(l + 0));
					Vertex* vertex1 = &primitiveVertices.at(primitiveIndices.at(l + 1));
					Vertex* vertex2 = &primitiveVertices.at(primitiveIndices.at(l + 2));

					glm::vec3 dPos1 = vertex1->position - vertex0->position;
					glm::vec3 dPos2 = vertex2->position - vertex0->position;

					glm::vec2 dUV1 = vertex1->uv - vertex0->uv;
					glm::vec2 dUV2 = vertex2->uv - vertex0->uv;

					float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

					glm::vec3 tangent = (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;

					vertex0->tangent += tangent;
					vertex1->tangent += tangent;
					vertex2->tangent += tangent;
				}
			}

			// Textures
			uint64_t materialID = 0;

			if (primitive->material != NULL) {
				Material primitiveMaterial;

				if (primitive->material->has_pbr_metallic_roughness) {
					cgltf_pbr_metallic_roughness pbrMetallicRoughness = primitive->material->pbr_metallic_roughness;
					cgltf_texture_view baseColorTextureView = pbrMetallicRoughness.base_color_texture;
					cgltf_texture* baseColorTexture = baseColorTextureView.texture;
					cgltf_float* baseColorFactor = pbrMetallicRoughness.base_color_factor;
					if (baseColorTexture != NULL) {
						cgltf_image* baseColorImage = baseColorTexture->image;

						if (textures.find(baseColorImage->uri) == textures.end()) {
							Image image;
							ImageTools::loadImage(FileTools::fileGetDirectory(filePath) + baseColorImage->uri, &image.image, VK_FORMAT_R8G8B8A8_SRGB, &image.mipmapLevels, &image.allocationId);
							ImageTools::createImageView(&image.imageView, image.image, 0, 1, 0, image.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
							ImageTools::createImageSampler(&image.imageSampler, image.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
							textures.emplace(baseColorImage->uri, image);
						}
						primitiveMaterial.diffuseKey = baseColorImage->uri;
					}
					else if (baseColorFactor != NULL) {
						std::string mapKey = std::to_string(baseColorFactor[0]) + std::to_string(baseColorFactor[1]) + std::to_string(baseColorFactor[2]) + std::to_string(baseColorFactor[3]);

						if (textures.find(mapKey) == textures.end()) {
							Image image;
							ImageTools::loadColor(baseColorFactor, &image.image, VK_FORMAT_R8G8B8A8_SRGB, &image.mipmapLevels, &image.allocationId);
							ImageTools::createImageView(&image.imageView, image.image, 0, 1, 0, image.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
							ImageTools::createImageSampler(&image.imageSampler, image.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
							textures.emplace(mapKey, image);
						}
						primitiveMaterial.diffuseKey = mapKey;
					}

					cgltf_texture_view metallicRoughnessTextureView = pbrMetallicRoughness.metallic_roughness_texture;
					cgltf_texture* metallicRoughnessTexture = metallicRoughnessTextureView.texture;
					cgltf_float metallicFactor = pbrMetallicRoughness.metallic_factor;
					cgltf_float roughnessFactor = pbrMetallicRoughness.roughness_factor;
					if (metallicRoughnessTexture != NULL) {
						cgltf_image* metallicRoughnessImage = metallicRoughnessTexture->image;

						if (textures.find(metallicRoughnessImage->uri) == textures.end()) {
							Image image;
							ImageTools::loadImage(FileTools::fileGetDirectory(filePath) + metallicRoughnessImage->uri, &image.image, VK_FORMAT_R8G8B8A8_UNORM, &image.mipmapLevels, &image.allocationId);
							ImageTools::createImageView(&image.imageView, image.image, 0, 1, 0, image.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
							ImageTools::createImageSampler(&image.imageSampler, image.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
							textures.emplace(metallicRoughnessImage->uri, image);
						}
						primitiveMaterial.metallicRoughnessKey = metallicRoughnessImage->uri;
					}
					else {
						std::string metallicRoughnessMapKey = std::to_string(metallicFactor) + std::to_string(roughnessFactor);
						float metallicRoughnessArray[4] = { 0.0f, roughnessFactor, metallicFactor, 1.0f };

						if (textures.find(metallicRoughnessMapKey) == textures.end()) {
							Image image;
							ImageTools::loadColor(metallicRoughnessArray, &image.image, VK_FORMAT_R8G8B8A8_UNORM, &image.mipmapLevels, &image.allocationId);
							ImageTools::createImageView(&image.imageView, image.image, 0, 1, 0, image.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
							ImageTools::createImageSampler(&image.imageSampler, image.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
							textures.emplace(metallicRoughnessMapKey, image);
						}
						primitiveMaterial.metallicRoughnessKey = metallicRoughnessMapKey;
					}
				}

				cgltf_texture_view normalTextureView = primitive->material->normal_texture;
				cgltf_texture* normalTexture = normalTextureView.texture;
				if (normalTexture != NULL) {
					cgltf_image* normalImage = normalTexture->image;

					if (textures.find(normalImage->uri) == textures.end()) {
						Image image;
						ImageTools::loadImage(FileTools::fileGetDirectory(filePath) + normalImage->uri, &image.image, VK_FORMAT_R8G8B8A8_UNORM, &image.mipmapLevels, &image.allocationId);
						ImageTools::createImageView(&image.imageView, image.image, 0, 1, 0, image.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
						ImageTools::createImageSampler(&image.imageSampler, image.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
						textures.emplace(normalImage->uri, image);
					}
					primitiveMaterial.normalKey = normalImage->uri;
				}

				cgltf_texture_view occlusionTextureView = primitive->material->occlusion_texture;
				cgltf_texture* occlusionTexture = occlusionTextureView.texture;
				if (occlusionTexture != NULL) {
					cgltf_image* occlusionImage = occlusionTexture->image;

					if (textures.find(occlusionImage->uri) == textures.end()) {
						Image image;
						ImageTools::loadImage(FileTools::fileGetDirectory(filePath) + occlusionImage->uri, &image.image, VK_FORMAT_R8G8B8A8_UNORM, &image.mipmapLevels, &image.allocationId);
						ImageTools::createImageView(&image.imageView, image.image, 0, 1, 0, image.mipmapLevels, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
						ImageTools::createImageSampler(&image.imageSampler, image.mipmapLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, VK_COMPARE_OP_ALWAYS);
						textures.emplace(occlusionImage->uri, image);
					}
					primitiveMaterial.occlusionKey = occlusionImage->uri;
				}

				materials.push_back(primitiveMaterial);

				materialID = static_cast<uint64_t>(materials.size() - 1);
			}

			// Primitive
			primitives.push_back({ firstIndex, indexCount, vertexOffset, materialID });

			vertices->insert(vertices->end(), primitiveVertices.begin(), primitiveVertices.end());
			indices->insert(indices->end(), primitiveIndices.begin(), primitiveIndices.end());

			*indexOffset += indexCount;
			*modelVertexOffset += vertexCount;
		}

		for (size_t l = 0; l < vertices->size(); l++) {
			Vertex* vertex = &vertices->at(l);

			vertex->tangent = glm::normalize(vertex->tangent);
		}

		modelMesh.primitives = primitives;

		if (node->skin != NULL) {
			cgltf_skin* skin = node->skin;

			cgltf_accessor* accessor = skin->inverse_bind_matrices;
			cgltf_buffer_view* buffer_view = accessor->buffer_view;
			char* buffer = static_cast<char*>(buffer_view->buffer->data);
			float* inverseBindMatrices = reinterpret_cast<float*>(buffer + accessor->offset + buffer_view->offset);

			std::vector<Bone> boneList;
			std::vector<cgltf_node*> nodeList;
			std::vector<glm::mat4> inverseBindMatrixList;
			for (size_t i = 0; i < skin->joints_count; i++) {
				cgltf_node* joint = skin->joints[i];
				glm::mat4 inverseBindMatrix = glm::make_mat4(inverseBindMatrices + (i * 16));

				nodeList.push_back(joint);
				inverseBindMatrixList.push_back(inverseBindMatrix);
			}

			loadglTFJoint(filePath, skin->joints[0], modelMatrix, glm::mat4(1.0f), nullptr, &boneList, nodeList, inverseBindMatrixList);

			modelMesh.skeleton = boneList[0];
			modelMesh.boneList = boneList;
		}

		meshes->push_back(modelMesh);
	}

	for (size_t i = 0; i < node->children_count; i++) {
		loadglTFNode(filePath, node->children[i], indexOffset, modelVertexOffset, modelMatrix, vertices, indices, meshes);
	}
}

void ModelLoader::loadglTFJoint(const std::string& filePath, cgltf_node* node, glm::mat4 globalTransform, glm::mat4 localTransform, Bone* hierarchy, std::vector<Bone>* boneList, std::vector<cgltf_node*> nodeList, std::vector<glm::mat4> inverseBindMatrixList) {
	if (node->has_matrix) {
		cgltf_float* matrix = node->matrix;
		localTransform = localTransform * glm::make_mat4(matrix);
	}
	else {
		if (node->has_translation) {
			cgltf_float* translation = node->translation;
			glm::mat4 meshTranslation = glm::mat4(1.0f);
			meshTranslation[3][0] = translation[0];
			meshTranslation[3][1] = translation[1];
			meshTranslation[3][2] = translation[2];
			localTransform = localTransform * meshTranslation;
		}

		if (node->has_rotation) {
			cgltf_float* rotation = node->rotation;
			glm::quat rotationQuaternion = glm::quat(rotation[0], rotation[1], rotation[2], rotation[3]);
			glm::mat4 meshRotation = glm::toMat4(rotationQuaternion);
			localTransform = localTransform * meshRotation;
		}

		if (node->has_scale) {
			cgltf_float* scale = node->scale;
			glm::mat4 meshScale = glm::mat4(1.0f);
			meshScale[0][0] = scale[0];
			meshScale[1][1] = scale[1];
			meshScale[2][2] = scale[2];
			localTransform = localTransform * meshScale;
		}
	}

	Bone bone = {};
	for (size_t i = 0; i < nodeList.size(); i++) {
		if (node == nodeList[i]) {
			bone.inverseBindMatrix = inverseBindMatrixList[i];
		}
	}

	bone.transformation = glm::inverse(globalTransform) * localTransform * bone.inverseBindMatrix;
	
	boneList->push_back(bone);
	for (size_t i = 0; i < node->children_count; i++) {
		loadglTFJoint(filePath, node->children[i], globalTransform, localTransform, &bone, boneList, nodeList, inverseBindMatrixList);
	}

	if (hierarchy != nullptr) {
		hierarchy->children.push_back(bone);
	}
}

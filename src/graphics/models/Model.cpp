#include "Model.h"
#include "../resources/RendererResources.h"
#include "../resources/ShaderResources.h"

void Model::init(std::string filePath) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	ModelLoader::load(filePath, &vertices, &indices, &meshes);

	Buffer stagingVertexBuffer;
	VkDeviceSize size = vertices.size() * sizeof(Vertex);
	BufferTools::createStagingBuffer(stagingVertexBuffer.buffer, stagingVertexBuffer.deviceMemory, size);
	void* vertexData;
	stagingVertexBuffer.map(0, size, &vertexData);
	memcpy(vertexData, vertices.data(), static_cast<size_t>(size));
	stagingVertexBuffer.unmap();
	BufferTools::createBuffer(vertexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer.allocationId);
	BufferTools::copyBuffer(stagingVertexBuffer.buffer, vertexBuffer.buffer, size);
	stagingVertexBuffer.destroy();

	Buffer stagingIndexBuffer;
	size = indices.size() * sizeof(uint32_t);
	BufferTools::createStagingBuffer(stagingIndexBuffer.buffer, stagingIndexBuffer.deviceMemory, size);
	void* indexData;
	stagingIndexBuffer.map(0, size, &indexData);
	memcpy(indexData, indices.data(), static_cast<size_t>(size));
	stagingIndexBuffer.unmap();
	BufferTools::createBuffer(indexBuffer.buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer.allocationId);
	BufferTools::copyBuffer(stagingIndexBuffer.buffer, indexBuffer.buffer, size);
	stagingIndexBuffer.destroy();

	for (Mesh& mesh : meshes) {
		mesh.boneBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		for (Buffer& buffer : mesh.boneBuffers) {
			NEIGE_ASSERT(mesh.boneList.size() <= MAX_BONES, "A mesh has more than " + std::to_string(MAX_BONES) + " bones.");

			BufferTools::createUniformBuffer(buffer.buffer, buffer.deviceMemory, sizeof(BoneUniformBufferObject));

			BoneUniformBufferObject bubo = {};
			for (size_t i = 0; i < mesh.boneList.size(); i++) {
				bubo.transformations[i] = mesh.boneList[i].transformation;
				bubo.inverseBindMatrices[i] = mesh.boneList[i].inverseBindMatrix;
			}

			void* data;
			buffer.map(0, sizeof(BoneUniformBufferObject), &data);
			memcpy(data, &bubo, sizeof(BoneUniformBufferObject));
			buffer.unmap();
		}
	}
}

void Model::destroy() {
	vertexBuffer.destroy();
	indexBuffer.destroy();
	for (Mesh& mesh : meshes) {
		for (Buffer& buffer : mesh.boneBuffers) {
			buffer.destroy();
		}
	}
}

void Model::draw(CommandBuffer* commandBuffer, uint32_t frameInFlightIndex, bool bindTextures) {
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer->commandBuffer, 0, 1, &vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(commandBuffer->commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	for (Mesh& mesh : meshes) {
		for (size_t i = 0; i < mesh.primitives.size(); i++) {
			if (bindTextures) {
				mesh.descriptorSets.at(i).at(frameInFlightIndex).bind(commandBuffer, 1);
			}
			vkCmdDrawIndexed(commandBuffer->commandBuffer, mesh.primitives[i].indexCount, 1, mesh.indexOffset + mesh.primitives[i].firstIndex, mesh.vertexOffset + mesh.primitives[i].vertexOffset, 0);
		}
	}
}

void Model::createDescriptorSets(GraphicsPipeline* graphicsPipeline) {
	for (Mesh& mesh : meshes) {
		mesh.descriptorSets.resize(mesh.primitives.size());
		for (size_t i = 0; i < mesh.primitives.size(); i++) {
			mesh.descriptorSets.at(i).resize(MAX_FRAMES_IN_FLIGHT);
			for (int j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
				mesh.descriptorSets.at(i).at(j).init(graphicsPipeline, 1);

				std::vector<VkWriteDescriptorSet> writesDescriptorSet;

				for (size_t k = 0; k < graphicsPipeline->sets[1].bindings.size(); k++) {
					std::string bindingName = graphicsPipeline->sets[1].bindings[k].name;
					if (bindingName == "colorMap") {
						std::string diffuseKey = materials[mesh.primitives[i].materialIndex].diffuseKey;
						if (diffuseKey == "") {
							diffuseKey = "defaultDiffuse";
						}
						VkDescriptorImageInfo diffuseInfo = {};
						diffuseInfo.sampler = textures.at(diffuseKey).imageSampler;
						diffuseInfo.imageView = textures.at(diffuseKey).imageView;
						diffuseInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

						VkWriteDescriptorSet diffuseWriteDescriptorSet = {};
						diffuseWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						diffuseWriteDescriptorSet.pNext = nullptr;
						diffuseWriteDescriptorSet.dstSet = mesh.descriptorSets.at(i).at(j).descriptorSet;
						diffuseWriteDescriptorSet.dstBinding = graphicsPipeline->sets[1].bindings[k].binding.binding;
						diffuseWriteDescriptorSet.dstArrayElement = 0;
						diffuseWriteDescriptorSet.descriptorCount = 1;
						diffuseWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						diffuseWriteDescriptorSet.pImageInfo = &diffuseInfo;
						diffuseWriteDescriptorSet.pBufferInfo = nullptr;
						diffuseWriteDescriptorSet.pTexelBufferView = nullptr;
						writesDescriptorSet.push_back(diffuseWriteDescriptorSet);
					}
					else if (bindingName == "normalMap") {
						std::string normalKey = materials[mesh.primitives[i].materialIndex].normalKey;
						if (normalKey == "") {
							normalKey = "defaultNormal";
						}
						VkDescriptorImageInfo normalInfo = {};
						normalInfo.sampler = textures.at(normalKey).imageSampler;
						normalInfo.imageView = textures.at(normalKey).imageView;
						normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

						VkWriteDescriptorSet normalWriteDescriptorSet = {};
						normalWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						normalWriteDescriptorSet.pNext = nullptr;
						normalWriteDescriptorSet.dstSet = mesh.descriptorSets.at(i).at(j).descriptorSet;
						normalWriteDescriptorSet.dstBinding = graphicsPipeline->sets[1].bindings[k].binding.binding;
						normalWriteDescriptorSet.dstArrayElement = 0;
						normalWriteDescriptorSet.descriptorCount = 1;
						normalWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						normalWriteDescriptorSet.pImageInfo = &normalInfo;
						normalWriteDescriptorSet.pBufferInfo = nullptr;
						normalWriteDescriptorSet.pTexelBufferView = nullptr;
						writesDescriptorSet.push_back(normalWriteDescriptorSet);
					}
					else if (bindingName == "metallicRoughnessMap") {
						std::string metallicRoughnessKey = materials[mesh.primitives[i].materialIndex].metallicRoughnessKey;
						if (metallicRoughnessKey == "") {
							metallicRoughnessKey = "defaultMetallicRoughness";
						}
						VkDescriptorImageInfo metallicRoughnessInfo = {};
						metallicRoughnessInfo.sampler = textures.at(metallicRoughnessKey).imageSampler;
						metallicRoughnessInfo.imageView = textures.at(metallicRoughnessKey).imageView;
						metallicRoughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

						VkWriteDescriptorSet metallicRoughnessWriteDescriptorSet = {};
						metallicRoughnessWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						metallicRoughnessWriteDescriptorSet.pNext = nullptr;
						metallicRoughnessWriteDescriptorSet.dstSet = mesh.descriptorSets.at(i).at(j).descriptorSet;
						metallicRoughnessWriteDescriptorSet.dstBinding = graphicsPipeline->sets[1].bindings[k].binding.binding;
						metallicRoughnessWriteDescriptorSet.dstArrayElement = 0;
						metallicRoughnessWriteDescriptorSet.descriptorCount = 1;
						metallicRoughnessWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						metallicRoughnessWriteDescriptorSet.pImageInfo = &metallicRoughnessInfo;
						metallicRoughnessWriteDescriptorSet.pBufferInfo = nullptr;
						metallicRoughnessWriteDescriptorSet.pTexelBufferView = nullptr;
						writesDescriptorSet.push_back(metallicRoughnessWriteDescriptorSet);
					}
					else if (bindingName == "emissiveMap") {
						std::string emissiveKey = materials[mesh.primitives[i].materialIndex].emissiveKey;
						if (emissiveKey == "") {
							emissiveKey = "defaultEmissive";
						}
						VkDescriptorImageInfo emissiveInfo = {};
						emissiveInfo.sampler = textures.at(emissiveKey).imageSampler;
						emissiveInfo.imageView = textures.at(emissiveKey).imageView;
						emissiveInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

						VkWriteDescriptorSet emissiveWriteDescriptorSet = {};
						emissiveWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						emissiveWriteDescriptorSet.pNext = nullptr;
						emissiveWriteDescriptorSet.dstSet = mesh.descriptorSets.at(i).at(j).descriptorSet;
						emissiveWriteDescriptorSet.dstBinding = graphicsPipeline->sets[1].bindings[k].binding.binding;
						emissiveWriteDescriptorSet.dstArrayElement = 0;
						emissiveWriteDescriptorSet.descriptorCount = 1;
						emissiveWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						emissiveWriteDescriptorSet.pImageInfo = &emissiveInfo;
						emissiveWriteDescriptorSet.pBufferInfo = nullptr;
						emissiveWriteDescriptorSet.pTexelBufferView = nullptr;
						writesDescriptorSet.push_back(emissiveWriteDescriptorSet);
					}
					else if (bindingName == "occlusionMap") {
						std::string occlusionKey = materials[mesh.primitives[i].materialIndex].occlusionKey;
						if (occlusionKey == "") {
							occlusionKey = "defaultOcclusion";
						}
						VkDescriptorImageInfo occlusionInfo = {};
						occlusionInfo.sampler = textures.at(occlusionKey).imageSampler;
						occlusionInfo.imageView = textures.at(occlusionKey).imageView;
						occlusionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

						VkWriteDescriptorSet occlusionWriteDescriptorSet = {};
						occlusionWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						occlusionWriteDescriptorSet.pNext = nullptr;
						occlusionWriteDescriptorSet.dstSet = mesh.descriptorSets.at(i).at(j).descriptorSet;
						occlusionWriteDescriptorSet.dstBinding = graphicsPipeline->sets[1].bindings[k].binding.binding;
						occlusionWriteDescriptorSet.dstArrayElement = 0;
						occlusionWriteDescriptorSet.descriptorCount = 1;
						occlusionWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						occlusionWriteDescriptorSet.pImageInfo = &occlusionInfo;
						occlusionWriteDescriptorSet.pBufferInfo = nullptr;
						occlusionWriteDescriptorSet.pTexelBufferView = nullptr;
						writesDescriptorSet.push_back(occlusionWriteDescriptorSet);
					}
					else if (bindingName == "bones") {
						VkDescriptorBufferInfo bonesInfo = {};
						bonesInfo.buffer = mesh.boneBuffers.at(j).buffer;
						bonesInfo.offset = 0;
						bonesInfo.range = sizeof(BoneUniformBufferObject);

						VkWriteDescriptorSet bonesWriteDescriptorSet = {};
						bonesWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						bonesWriteDescriptorSet.pNext = nullptr;
						bonesWriteDescriptorSet.dstSet = mesh.descriptorSets.at(i).at(j).descriptorSet;
						bonesWriteDescriptorSet.dstBinding = graphicsPipeline->sets[1].bindings[k].binding.binding;
						bonesWriteDescriptorSet.dstArrayElement = 0;
						bonesWriteDescriptorSet.descriptorCount = 1;
						bonesWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						bonesWriteDescriptorSet.pImageInfo = nullptr;
						bonesWriteDescriptorSet.pBufferInfo = &bonesInfo;
						bonesWriteDescriptorSet.pTexelBufferView = nullptr;
						writesDescriptorSet.push_back(bonesWriteDescriptorSet);
					}
				}

				mesh.descriptorSets.at(i).at(j).update(writesDescriptorSet);
			}
		}
	}
}

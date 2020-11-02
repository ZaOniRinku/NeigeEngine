#include "Model.h"
#include "../resources/RendererResources.h"
#include "../resources/ShaderResources.h"

void Model::init(std::string filePath) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	ModelLoader::load(filePath, &vertices, &indices, &primitives);
	
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
}

void Model::destroy() {
	vertexBuffer.destroy();
	indexBuffer.destroy();
}

void Model::draw(CommandBuffer* commandBuffer, bool bindTextures) {
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer->commandBuffer, 0, 1, &vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(commandBuffer->commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	for (size_t i = 0; i < primitives.size(); i++) {
		if (bindTextures) {
			descriptorSets.at(i).bind(commandBuffer, 1);
		}
		vkCmdDrawIndexed(commandBuffer->commandBuffer, primitives[i].indexCount, 1, primitives[i].firstIndex, 0, 0);
	}
}

void Model::createDescriptorSets(GraphicsPipeline* graphicsPipeline) {
	descriptorSets.resize(primitives.size());
	for (size_t i = 0; i < primitives.size(); i++) {
		descriptorSets.at(i).init(graphicsPipeline, 1);

		std::string diffuseKey = materials[primitives[i].materialIndex].diffuseKey;
		if (diffuseKey == "") {
			diffuseKey = "defaultDiffuse";
		}
		VkDescriptorImageInfo diffuseInfo = {};
		diffuseInfo.sampler = textures.at(diffuseKey).imageSampler;
		diffuseInfo.imageView = textures.at(diffuseKey).imageView;
		diffuseInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::string normalKey = materials[primitives[i].materialIndex].normalKey;
		if (normalKey == "") {
			normalKey = "defaultNormal";
		}
		VkDescriptorImageInfo normalInfo = {};
		normalInfo.sampler = textures.at(normalKey).imageSampler;
		normalInfo.imageView = textures.at(normalKey).imageView;
		normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::string metallicRoughnessKey = materials[primitives[i].materialIndex].metallicRoughnessKey;
		if (metallicRoughnessKey == "") {
			metallicRoughnessKey = "defaultMetallicRoughness";
		}
		VkDescriptorImageInfo metallicRoughnessInfo = {};
		metallicRoughnessInfo.sampler = textures.at(metallicRoughnessKey).imageSampler;
		metallicRoughnessInfo.imageView = textures.at(metallicRoughnessKey).imageView;
		metallicRoughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::string occlusionKey = materials[primitives[i].materialIndex].occlusionKey;
		if (occlusionKey == "") {
			occlusionKey = "defaultOcclusion";
		}
		VkDescriptorImageInfo occlusionInfo = {};
		occlusionInfo.sampler = textures.at(occlusionKey).imageSampler;
		occlusionInfo.imageView = textures.at(occlusionKey).imageView;
		occlusionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector<VkWriteDescriptorSet> writesDescriptorSet;

		VkWriteDescriptorSet diffuseWriteDescriptorSet = {};
		diffuseWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		diffuseWriteDescriptorSet.pNext = nullptr;
		diffuseWriteDescriptorSet.dstSet = descriptorSets[i].descriptorSet;
		diffuseWriteDescriptorSet.dstBinding = 0;
		diffuseWriteDescriptorSet.dstArrayElement = 0;
		diffuseWriteDescriptorSet.descriptorCount = 1;
		diffuseWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		diffuseWriteDescriptorSet.pImageInfo = &diffuseInfo;
		diffuseWriteDescriptorSet.pBufferInfo = nullptr;
		diffuseWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(diffuseWriteDescriptorSet);

		VkWriteDescriptorSet normalWriteDescriptorSet = {};
		normalWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		normalWriteDescriptorSet.pNext = nullptr;
		normalWriteDescriptorSet.dstSet = descriptorSets[i].descriptorSet;
		normalWriteDescriptorSet.dstBinding = 1;
		normalWriteDescriptorSet.dstArrayElement = 0;
		normalWriteDescriptorSet.descriptorCount = 1;
		normalWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalWriteDescriptorSet.pImageInfo = &normalInfo;
		normalWriteDescriptorSet.pBufferInfo = nullptr;
		normalWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(normalWriteDescriptorSet);

		VkWriteDescriptorSet metallicRoughnessWriteDescriptorSet = {};
		metallicRoughnessWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		metallicRoughnessWriteDescriptorSet.pNext = nullptr;
		metallicRoughnessWriteDescriptorSet.dstSet = descriptorSets[i].descriptorSet;
		metallicRoughnessWriteDescriptorSet.dstBinding = 2;
		metallicRoughnessWriteDescriptorSet.dstArrayElement = 0;
		metallicRoughnessWriteDescriptorSet.descriptorCount = 1;
		metallicRoughnessWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		metallicRoughnessWriteDescriptorSet.pImageInfo = &metallicRoughnessInfo;
		metallicRoughnessWriteDescriptorSet.pBufferInfo = nullptr;
		metallicRoughnessWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(metallicRoughnessWriteDescriptorSet);

		VkWriteDescriptorSet occlusionWriteDescriptorSet = {};
		occlusionWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		occlusionWriteDescriptorSet.pNext = nullptr;
		occlusionWriteDescriptorSet.dstSet = descriptorSets[i].descriptorSet;
		occlusionWriteDescriptorSet.dstBinding = 3;
		occlusionWriteDescriptorSet.dstArrayElement = 0;
		occlusionWriteDescriptorSet.descriptorCount = 1;
		occlusionWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		occlusionWriteDescriptorSet.pImageInfo = &occlusionInfo;
		occlusionWriteDescriptorSet.pBufferInfo = nullptr;
		occlusionWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(occlusionWriteDescriptorSet);

		descriptorSets[i].update(writesDescriptorSet);
	}
}

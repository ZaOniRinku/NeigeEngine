#include "Model.h"

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

void Model::draw(CommandBuffer* commandBuffer) {
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer->commandBuffer, 0, 1, &vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(commandBuffer->commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	for (size_t i = 0; i < primitives.size(); i++) {
		vkCmdDrawIndexed(commandBuffer->commandBuffer, primitives[i].indexCount, 1, primitives[i].firstIndex, 0, 0);
	}
}

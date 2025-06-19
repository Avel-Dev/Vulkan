#pragma once
#include "VulkanEngine/Buffer/VertexBuffer.h"
#include <tinygltf/tiny_gltf.h>

namespace CHIKU
{
	struct VertexAttributeInfoData;
	
	namespace Utils
	{
        std::vector<VertexAttributeInfoData> GetVertexLayout(const tinygltf::Model& model);
		VertexBufferLayout CreateBufferLayout(const tinygltf::Model& model, const tinygltf::Primitive& primitive);
		void CreateVertexData(const VertexAttributeInfoData& infoData, std::vector<uint8_t>& outBuffer);
		void PrintVertexData(const std::vector<uint8_t>& buffer, const VertexAttributeInfoData& infoData);
	}
}
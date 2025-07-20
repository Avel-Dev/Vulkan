#pragma once
#include "Renderer/Assets/Asset.h"
#include "Renderer/Buffer/UniformBuffer.h"
#include "Renderer/Buffer/VertexBuffer.h"

#include <spirv_reflect.h>
#include <bitset>

namespace CHIKU
{
	namespace Utils 
	{
		void PrintUniformSetLayout(const UniformBufferDescription& uniformSets);
		UniformPlainDataType GetSPIRVDataType(const SpvReflectBlockVariable& member);
		UniformOpaqueDataType ConvertToOpaqueType(const SpvReflectDescriptorBinding* binding);

		bool IsVertexShader(const AssetPath& shaderPath);
		void ProcessSPIRV(const std::vector<AssetPath>& shaderCodes, UniformBufferDescription& description, std::bitset<ATTR_COUNT>& inputAttribute);
		void GetUniformDescription(UniformBufferDescription& uniformBufferSet, const SpvReflectShaderModule& spirv);
		void GetInputAttributes(std::bitset<ATTR_COUNT>& inputAttribute, const SpvReflectShaderModule& spirv);

		VkShaderStageFlags MapToVulkanShaderStageFlags(const ShaderStageBits& stageBits);
	}
}
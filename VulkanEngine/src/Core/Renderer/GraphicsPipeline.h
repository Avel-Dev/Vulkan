#pragma once
#include "VulkanHeader.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <vector>
#include <functional>
	
namespace CHIKU
{
	struct PipelineKey
	{
		ShaderID shaderID;
		VertexLayoutPreset inputDescription;
		MaterialPresets materialPreset;

		bool operator==(const PipelineKey& other) const
		{
			return shaderID == other.shaderID &&
				inputDescription == other.inputDescription &&
				materialPreset == other.materialPreset;
		}
	};

	class GraphicsPipeline
	{
	public:
		void Init();
		void Bind(const Material& material, const VertexBuffer& vertexbuffers);
		void CleanUp();

	private:
		struct Pipeline
		{
			VkPipelineLayout PipelineLayout;
			VkPipeline GraphicsPipeline;
		};

		Pipeline GetOrCreateGraphicsPipeline(PipelineKey key, const Material& material, const VertexBuffer& vertexBuffer);
		Pipeline CreateGraphicsPipeline(const VkPipelineShaderStageCreateInfo* pipelineStages, VertexBuffer::VertexInputDescription description, VkDescriptorSetLayout layout);

	private:
		static std::unordered_map<PipelineKey, Pipeline> sm_GrphicsPipeline;
	};
}

namespace std 
{
	template <>
	struct hash<CHIKU::ShaderID> 
	{
		size_t operator()(const CHIKU::ShaderID& id) const noexcept 
		{
			return static_cast<size_t>(id);
		}
	};

	template <>
	struct hash<CHIKU::VertexLayoutPreset> 
	{
		size_t operator()(const CHIKU::VertexLayoutPreset& layout) const noexcept 
		{
			return static_cast<size_t>(layout);
		}
	};

	template <>
	struct hash<CHIKU::MaterialPresets> 
	{
		size_t operator()(const CHIKU::MaterialPresets& preset) const noexcept 
		{
			return static_cast<size_t>(preset);
		}
	};

	template <>
	struct hash<CHIKU::PipelineKey>
	{
		std::size_t operator()(const CHIKU::PipelineKey& key) const noexcept
		{
			std::size_t h = 0;

			// hash_combine utility
			auto hash_combine = [](std::size_t& seed, std::size_t val) {
				seed ^= val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				};

			hash_combine(h, std::hash<CHIKU::ShaderID>{}(key.shaderID));
			hash_combine(h, std::hash<CHIKU::VertexLayoutPreset>{}(key.inputDescription));
			hash_combine(h, std::hash<CHIKU::MaterialPresets>{}(key.materialPreset));
			return h;
		}
	};
}
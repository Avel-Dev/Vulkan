#pragma once
#include "VulkanHeader.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <vector>
	
namespace CHIKU
{
	struct PipelineStateInfo
	{
		std::string VertexShader;
		std::string FragmentShader;
	};

	class GraphicsPipeline
	{
	public:
		void Init();
		void Bind();
		void CleanUp();

	private:
		void LoadModel();
		void CreateGraphicsPipeline();
		void GetOrCreateGraphicsPipeline();

	private:
		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;
		Material m_Material;

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};
}
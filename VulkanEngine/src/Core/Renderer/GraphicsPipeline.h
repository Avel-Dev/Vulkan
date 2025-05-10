#pragma once
#include "VulkanHeader.h"
#include "Renderer/Buffer.h"
#include "Renderer/Material.h"

namespace CHIKU
{
	struct PipelineDescriptor
	{

	};

	class GraphicsPipeline 
	{
		void Set(MaterialLayoutPreset MaterialPreset, Buffer::VertexLayoutPreset vertexPreset);

	private:
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};
}
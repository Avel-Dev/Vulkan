#pragma once
#include "VulkanHeader.h"
#include "GraphicsPipeline.h"
#include <string>

namespace CHIKU
{
	class Renderer
	{
	public:
		static Renderer* s_Instance;
		void Init();
		void Draw();
		void CleanUp();

	private:
		GraphicsPipeline m_GraphicsPipeline;
	};
}
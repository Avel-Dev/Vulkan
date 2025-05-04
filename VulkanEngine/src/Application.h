#pragma once
#include "Core/VulkanEngine.h"

namespace CHIKU
{
	class Application
	{
	public:
		void Init();
		void Run();
		void CleanUp();

	private:
		VulkanEngine m_Engine;
	};
}
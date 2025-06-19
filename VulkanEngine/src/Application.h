#pragma once
#include "VulkanEngine/VulkanEngine.h"
#include "VulkanEngine/Window.h"

namespace CHIKU
{
	class Application
	{
	public:
		Application();
		void Init();
		void Run();
		void Render();
		void CleanUp();

	private:
		Window m_Window;
		VulkanEngine m_Engine;
	};
}
#pragma once
#include "Core/VulkanEngine/VulkanEngine.h"
#include "Core/VulkanEngine/Window.h"

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
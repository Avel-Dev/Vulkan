#pragma once
#include "Core/VulkanEngine/VulkanEngine.h"
#include "Core/Renderer/Renderer.h"
#include "Core/VulkanEngine/Window.h"

namespace CHIKU
{
	class Application
	{
	public:
		Application();
		void Init();
		void Run();
		void CleanUp();

	private:
		Renderer m_Renderer;
		Window m_Window;
		VulkanEngine m_Engine;
	};
}
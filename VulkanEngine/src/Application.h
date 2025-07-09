#pragma once
#include "VulkanEngine/VulkanEngine.h"
#include "VulkanEngine/Window.h"
#include "VulkanEngine/Assets/ModelAsset.h"

namespace CHIKU
{
	class Application
	{
	public:
		Application() = default;
		void Init();
		void Run();
		void Render();
		void CleanUp();

	private:
		Window m_Window;
		VulkanEngine m_Engine;
		std::shared_ptr<ModelAsset> m_Model;
	};
}
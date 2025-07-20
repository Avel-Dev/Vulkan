#pragma once
#include "Renderer/Renderer.h"
#include "Renderer/Window.h"
#include "Renderer/Assets/ModelAsset.h"

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
		Renderer m_Renderer;
		std::shared_ptr<ModelAsset> m_Model;
	};
}
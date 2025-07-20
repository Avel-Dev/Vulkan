#include "Application.h"
#include "Utils/ShaderUtils.h"
#include "Renderer/DescriptorPool.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Assets/ModelAsset.h"
#include "Renderer/Assets/AssetManager.h"

namespace CHIKU
{
	void Application::Init()
	{
		ZoneScoped;    // Profile this block

		m_Window.Init();
		m_Renderer.Init(m_Window.GetWindow());
		AssetManager::Init();
		DescriptorPool::Init();
		GraphicsPipeline::Init();

		//AssetManager::AddShader({ "Shaders/Unlit/unlit.vert", "Shaders/Unlit/unlit.frag" });
		AssetManager::AddShader({ "Shaders/Defaultlit/defaultlit.vert", "Shaders/Defaultlit/defaultlit.frag" });
		AssetHandle model = AssetManager::AddModel("Models/Y Bot/Y Bot.gltf");

		std::shared_ptr<Asset> asset = AssetManager::GetAsset(model);
		m_Model = std::dynamic_pointer_cast<ModelAsset>(asset);

	}

	void Application::Run()
	{
		ZoneScoped;    // Profile this block
		while (!m_Window.WindowShouldClose())
		{
			FrameMark;
		}
	}

	//Renderer Thread Process
	void Application::Render()
	{
		ZoneScoped;

		while (!m_Window.WindowShouldClose())
		{
			FrameMark;
			m_Window.WindowPoolEvent();
			m_Renderer.BeginFrame();
			m_Model->Draw();
			m_Renderer.EndFrame();
		}
	}

	void Application::CleanUp()
	{
		ZoneScoped;

		m_Renderer.Wait();
		DescriptorPool::CleanUp();
		AssetManager::CleanUp();
		GraphicsPipeline::CleanUp();
		m_Renderer.CleanUp();
	}
}
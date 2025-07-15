#include "Application.h"
#include "Renderer/Renderer.h"
#include "Utils/ShaderUtils.h"
#include "VulkanEngine/DescriptorPool.h"
#include "VulkanEngine/GraphicsPipeline.h"
#include "VulkanEngine/Assets/ModelAsset.h"
#include "VulkanEngine/Assets/AssetManager.h"

namespace CHIKU
{
	void Application::Init()
	{
		ZoneScoped;    // Profile this block

		m_Window.Init();
		m_Engine.Init(m_Window.GetWindow());
		Renderer::Init();
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
			m_Engine.BeginFrame();
			m_Model->Draw();
			Renderer::Draw();
			m_Engine.EndFrame();
		}
	}

	void Application::CleanUp()
	{
		ZoneScoped;

		m_Engine.Wait();
		Renderer::CleanUp();
		DescriptorPool::CleanUp();
		AssetManager::CleanUp();
		GraphicsPipeline::CleanUp();
		m_Engine.CleanUp();
	}
}
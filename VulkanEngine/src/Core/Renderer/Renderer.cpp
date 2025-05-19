#include "Renderer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Shader.h"
#include <fstream>
#include <iostream>

namespace CHIKU
{
    Renderer* Renderer::s_Instance = new Renderer();

	void Renderer::Init()
	{
		ShaderManager::Init();
		m_GraphicsPipeline.Init();
	}

	void Renderer::Draw()
	{
		m_GraphicsPipeline.Bind();
		vkCmdDrawIndexed(VulkanEngine::GetCommandBuffer(),36,1,0,0,0);
	}

	void Renderer::CleanUp()
	{
		ShaderManager::Cleanup();
		m_GraphicsPipeline.CleanUp();
	}

}
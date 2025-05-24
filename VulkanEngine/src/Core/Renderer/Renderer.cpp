#include "Renderer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Shader.h"
#include <tiny_obj_loader.h>
#include <fstream>
#include <iostream>

namespace CHIKU
{
    Renderer* Renderer::s_Instance = new Renderer();

	void Renderer::Init()
	{
		VertexBuffer::Init();
		ShaderManager::Init();
		m_GraphicsPipeline.Init();

		m_Material.CreateMaterial(MaterialPresets::Unlit);
		LoadModel();
	}

    void Renderer::LoadModel()
    {
        std::vector<uint32_t> indices;
        std::vector<glm::vec3> data;
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (SOURCE_DIR + "models/viking_room.obj").c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                glm::vec3 position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                glm::vec3 color = { 1.0f, 1.0f, 1.0f };

                data.emplace_back(position);
                data.emplace_back(color);

                indices.push_back((uint32_t)indices.size());
            }
        }

        std::vector<uint8_t> vertexBytes(
            reinterpret_cast<uint8_t*>(data.data()),
            reinterpret_cast<uint8_t*>(data.data()) + data.size() * sizeof(glm::vec3)
        );

        m_VertexBuffer.SetLayout(VertexLayoutPreset::UnLitMesh);
        m_VertexBuffer.CreateVertexBuffer(vertexBytes);
        m_IndexBuffer.CreateIndexBuffer(indices);
    }

	void Renderer::Draw()
	{
        m_Material.Update();
		m_GraphicsPipeline.Bind(m_Material,m_VertexBuffer);
        m_IndexBuffer.Bind();
		vkCmdDrawIndexed(VulkanEngine::GetCommandBuffer(),m_IndexBuffer.GetCount(), 1, 0, 0, 0);
	}

	void Renderer::CleanUp()
	{
        m_Material.CleanUp();
        m_VertexBuffer.CleanUp();
        m_IndexBuffer.CleanUp();

		ShaderManager::Cleanup();
		Material::StaticCleanUp();

		m_GraphicsPipeline.CleanUp();
	}

}
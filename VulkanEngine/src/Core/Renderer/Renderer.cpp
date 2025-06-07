#include "Renderer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include <tiny_obj_loader.h>
#include <fstream>
#include <iostream>

namespace CHIKU
{
    Renderer* Renderer::s_Instance = new Renderer();

	void Renderer::Init()
	{
        ZoneScoped;

		VertexBuffer::Init();
		ShaderManager::Init();
        UniformBuffer::Init();
		m_GraphicsPipeline.Init();

        m_Material.CreateMaterial(MaterialPresets::Unlit);

        MaterialTextuers textures = {
            UniformOpaqueDataType::DIFFUSE_TEXTURE,
            "models/Texture1.png"
        };

        m_Material.CreateTexture(&textures, 1);
		LoadModel();
	}

    void Renderer::LoadModel()
    {
        ZoneScoped;

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

                glm::vec3 texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
                    1.0f };

                data.emplace_back(position);
                data.emplace_back(texCoord);

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
        ZoneScoped;

        UniformBuffer::Update();
		m_GraphicsPipeline.Bind(m_Material,m_VertexBuffer);
        m_IndexBuffer.Bind();
		vkCmdDrawIndexed(VulkanEngine::GetCommandBuffer(),m_IndexBuffer.GetCount(), 1, 0, 0, 0);
	}

	void Renderer::CleanUp()
	{
        ZoneScoped;

        m_Material.CleanUp();
        m_VertexBuffer.CleanUp();
        m_IndexBuffer.CleanUp();

        UniformBuffer::CleanUp();
		ShaderManager::Cleanup();

		m_GraphicsPipeline.CleanUp();
	}

}
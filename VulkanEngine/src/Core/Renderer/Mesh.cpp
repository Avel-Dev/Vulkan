#include "Mesh.h"
#include <tiny_obj_loader.h>

namespace CHIKU
{
    Mesh::Mesh() : m_VertexBuffer(Buffer::VertexLayoutPreset::StaticMesh)
    {
    }
    Mesh::~Mesh()
    {
        CleanUp();
    }

    void Mesh::CleanUp()
    {
        m_VertexBuffer.CleanUp();
    }

    void Mesh::LoadMesh(const tinyobj::attrib_t& attrib,const std::vector<tinyobj::shape_t>& shapes)
	{
         Buffer::VertexBufferLayout Layout = m_VertexBuffer.GetLayout();
         m_VertexData.clear();
        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                std::vector<uint8_t> data(Layout.Stride);
                size_t offset = 0;

                for (const auto& attr : Layout.VertexElements)
                {
                    offset = attr.Offset;

                    switch (attr.AttributeType)
                    {
                    case Buffer::VertexAttributeType::Vec3: // Example for position
                        if (attr.ElementName == VERTEX_FIELD_POSITION)
                        {
                            float pos[3] = {
                                attrib.vertices[3 * index.vertex_index + 0],
                                attrib.vertices[3 * index.vertex_index + 1],
                                attrib.vertices[3 * index.vertex_index + 2]
                            };

                            std::memcpy(data.data() + offset, pos, sizeof(float) * 3);
                        }
                        else if (attr.ElementName == VERTEX_FIELD_NORMAL)
                        {
                            float norm[3] = {
                                attrib.normals[3 * index.normal_index + 0],
                                attrib.normals[3 * index.normal_index + 1],
                                attrib.normals[3 * index.normal_index + 2]
                            };
                            memcpy(data.data() + offset, norm, sizeof(float) * 3);
                        }
                        break;

                    case Buffer::VertexAttributeType::Vec2:
                        if (attr.ElementName == VERTEX_FIELD_TEXCOORD)
                        {
                            float uv[2] = {
                                attrib.texcoords[2 * index.texcoord_index + 0],
                                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                            };
                            memcpy(data.data() + offset, uv, sizeof(float) * 2);
                        }
                        break;

                    case Buffer::VertexAttributeType::Vec4:
                        if (attr.ElementName == VERTEX_FIELD_COLOR)
                        {
                            float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // default
                            memcpy(data.data() + offset, color, sizeof(float) * 4);
                        }
                        break;

                        // Add more cases if needed
                    }
                }

                m_VertexData.insert(m_VertexData.end(), data.begin(), data.end());
                m_Indices.push_back(static_cast<uint32_t>(m_Indices.size()));
            }
        }

        m_VertexBuffer.SetData(m_VertexData);
	}
}

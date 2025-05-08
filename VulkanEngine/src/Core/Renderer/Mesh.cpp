#include "Mesh.h"
#include <tiny_obj_loader.h>

namespace CHIKU
{
    Mesh::Mesh() : m_VertexBuffer(Buffer::VertexLayoutPreset::StaticMesh)
    {
    }
    void Mesh::LoadMesh(const tinyobj::attrib_t& attrib,const std::vector<tinyobj::shape_t>& shapes)
	{
         Buffer::VertexBufferLayout Layout = m_VertexBuffer.GetLayout();

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                uint8_t* vertexPtr = new uint8_t[Layout.Stride];
                size_t offset = 0;

                for (const auto& attr : Layout.VertexElements)
                {
                    switch (attr.AttributeType)
                    {
                    case Buffer::VertexAttributeType::Vec3: // Example for position
                        if (attr.ElementName == "POSITION")
                        {
                            float pos[3] = {
                                attrib.vertices[3 * index.vertex_index + 0],
                                attrib.vertices[3 * index.vertex_index + 1],
                                attrib.vertices[3 * index.vertex_index + 2]
                            };
                            memcpy(vertexPtr + offset, pos, sizeof(float) * 3);
                        }
                        else if (attr.ElementName == "NORMAL")
                        {
                            float norm[3] = {
                                attrib.normals[3 * index.normal_index + 0],
                                attrib.normals[3 * index.normal_index + 1],
                                attrib.normals[3 * index.normal_index + 2]
                            };
                            memcpy(vertexPtr + offset, norm, sizeof(float) * 3);
                        }
                        break;

                    case Buffer::VertexAttributeType::Vec2:
                        if (attr.ElementName == "TEXCOORD")
                        {
                            float uv[2] = {
                                attrib.texcoords[2 * index.texcoord_index + 0],
                                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                            };
                            memcpy(vertexPtr + offset, uv, sizeof(float) * 2);
                        }
                        break;

                    case Buffer::VertexAttributeType::Vec4:
                        if (attr.ElementName == "COLOR")
                        {
                            float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // default
                            memcpy(vertexPtr + offset, color, sizeof(float) * 4);
                        }
                        break;

                        // Add more cases if needed
                    }

                    offset += attr.Offset;
                }

                m_VertexData.insert(m_VertexData.end(), vertexPtr, vertexPtr + Layout.Stride);
                m_Indices.push_back(static_cast<uint32_t>(m_Indices.size()));

                delete[] vertexPtr;
            }
        }

        m_VertexBuffer.SetData(m_VertexData);
	}
}

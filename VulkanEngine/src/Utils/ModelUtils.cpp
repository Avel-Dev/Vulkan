#include "ModelUtils.h"
#include "BufferUtils.h"
#include <iostream>

namespace CHIKU
{
    namespace Utils
    {
        void FinalizeLayout(GLTFVertexBufferLayout& layout)
        {
            ZoneScoped;

            uint32_t offset = 0;
            for (auto& attr : layout.VertexElements)
            {
                attr.Offset = offset;
                attr.size = static_cast<uint32_t>(GetAttributeSize(attr.ComponentType, attr.AttributeType));
                offset += attr.size;
            }
            layout.Stride = offset;
        }

        std::vector<GLTFVertexBufferMetaData> GetVertexLayout(const tinygltf::Model& model)
        {
            std::vector<GLTFVertexBufferMetaData> layouts;
            layouts.resize(model.meshes.size());
            int i = 0;

            for (auto& mesh : model.meshes)
            {
                for (auto& primitive : mesh.primitives)
                {
                    auto it = primitive.attributes.find(std::string(VertexAttributesArray[0]));
                    layouts[i].Count = model.accessors[it->second].count;
                    layouts[i].Layout = CreateBufferLayout(model, primitive);
                    FinalizeLayout(layouts[i].Layout);
                }

                i++;
            }

            return layouts;
        }

        GLTFVertexBufferLayout CreateBufferLayout(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
        {
            GLTFVertexBufferLayout layout;
            auto& mask = layout.mask;

            int i = 0;
            layout.VertexElements.resize(primitive.attributes.size());

            for (auto& attrname : VertexAttributesArray)
            {
                auto it = primitive.attributes.find(std::string(attrname));
                if (it == primitive.attributes.end()) 
                {
                    continue;
                }

                auto& vertexElements = layout.VertexElements[i];
                mask.set(static_cast<size_t>(i)); // if order matches ATTR enum

                int accessorIndex = it->second;

                const tinygltf::Accessor& accessor = model.accessors[accessorIndex];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];
                vertexElements.Data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    vertexElements.ComponentType = VertexComponentType::Float;
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    vertexElements.ComponentType = VertexComponentType::Short;
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    vertexElements.ComponentType = VertexComponentType::Int;
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    vertexElements.ComponentType = VertexComponentType::Byte;
                }

                if (accessor.type == TINYGLTF_TYPE_SCALAR)
                {
                    vertexElements.AttributeType = VertexAttributeType::SCALAR;
                }
                else if (accessor.type == TINYGLTF_TYPE_VEC2)
                {
                    vertexElements.AttributeType = VertexAttributeType::Vec2;
                }
                else if (accessor.type == TINYGLTF_TYPE_VEC3)
                {
                    vertexElements.AttributeType = VertexAttributeType::Vec3;
                }
                else if (accessor.type == TINYGLTF_TYPE_VEC4)
                {
                    vertexElements.AttributeType = VertexAttributeType::Vec4;
                }

                //std::cout << "  Count: " << accessor.count << "\n";
                //std::cout << "  Stride: " << (bufferView.byteStride ? bufferView.byteStride : tinygltf::GetNumComponentsInType(accessor.type) * tinygltf::GetComponentSizeInBytes(accessor.componentType)) << "\n";
                //std::cout << "  Offset in BufferView: " << accessor.byteOffset << "\n";
                //std::cout << "  BufferView Offset: " << bufferView.byteOffset << "\n";
                //std::cout << "  Buffer Index: " << bufferView.buffer << "\n";
                //std::cout << "---------------------------\n";

                i++;
            }

            return layout;
        }

        void CreateVertexData(const GLTFVertexBufferMetaData& infoData, std::vector<uint8_t>& outBuffer)
        {
            outBuffer.resize(infoData.Count * infoData.Layout.Stride);

            for (int i = 0; i < infoData.Count; ++i)
            {
                uint8_t* dstVertex = outBuffer.data() + i * infoData.Layout.Stride;

                for (const auto& attrib : infoData.Layout.VertexElements)
                {
                    const uint8_t* src = attrib.Data + i * attrib.size;
                    uint8_t* dst = dstVertex + attrib.Offset;
                    std::memcpy(dst, src, attrib.size);
                }
            }
        }

        const char* ToString(VertexComponentType compType,VertexAttributeType attribType)
        {
            switch (attribType)
            {
            case VertexAttributeType::SCALAR: return "Scalar";
            case VertexAttributeType::Vec2 : return "Vec2";
            case VertexAttributeType::Vec3: return "Vec3";
            case VertexAttributeType::Vec4: return "Vec4";
                // Add others as needed
            default: return "Unknown";
            }

            switch (compType)
            {
            case VertexComponentType::Float:    return "Float";
            case VertexComponentType::Byte:     return "Byte";
            case VertexComponentType::Int:      return "Int";
            case VertexComponentType::Short:    return "Short";
                            // Add others as needed
            default: return "Unknown";
            }
        }

        void PrintVertexData(const std::vector<uint8_t>& buffer, const GLTFVertexBufferMetaData& infoData)
        {
            for (int i = 0; i < infoData.Count; ++i)
            {
                const uint8_t* vertexPtr = buffer.data() + i * infoData.Layout.Stride;
                std::cout << "Vertex " << i << ": ";

                for (const auto& attrib : infoData.Layout.VertexElements)
                {
                    const uint8_t* attrPtr = vertexPtr + attrib.Offset;

                    std::cout << "[" << ToString(attrib.ComponentType,attrib.AttributeType) << "] ";

                    // Print each component as float (assuming 4-byte floats)
                    for (int j = 0; j < attrib.size / sizeof(float); ++j)
                    {
                        float value;
                        std::memcpy(&value, attrPtr + j * sizeof(float), sizeof(float));
                        std::cout << value << " ";
                    }

                    std::cout << " | ";
                }

                std::cout << "\n";
            }
        }

        VertexBufferMetaData ConvertGLTFInfoToVertexInfo(const GLTFVertexBufferMetaData& gltfInfo)
        {
            VertexBufferMetaData result;
            result.Count = gltfInfo.Count;
            result.Layout.Stride = gltfInfo.Layout.Stride;
            result.Layout.mask = gltfInfo.Layout.mask;

            result.Layout.VertexElements.reserve(gltfInfo.Layout.VertexElements.size());
            for (const auto& attr : gltfInfo.Layout.VertexElements)
            {
                VertexAttribute convertedAttr;
                convertedAttr.Offset = attr.Offset;
                convertedAttr.size = attr.size;
                convertedAttr.ComponentType = attr.ComponentType;
                convertedAttr.AttributeType = attr.AttributeType;

                result.Layout.VertexElements.push_back(convertedAttr);
            }

            return result;
        }

        bool IsGLTFFormat(const AssetPath& path)
        {
            std::string ext = path.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return ext == ".gltf" || ext == ".glb";
        }

        AssetPath ConvertToGLTF(const AssetPath& modelAsset)
        {
            if (Utils::IsGLTFFormat(modelAsset))
            {
                return modelAsset;
            }

            ZoneScoped;

            std::string stem = modelAsset.stem().string();
            AssetPath gltfPath = SOURCE_DIR + "Models/" + (stem + ".gltf");

            // Build the command
            std::string command = (SOURCE_DIR + "tools/FBX2glTF.exe")
                + " --input \"" + SOURCE_DIR + modelAsset.string() + "\""
                + " --output \"" + gltfPath.string() + "\"";

            int result = std::system(command.c_str());

            if (result != 0)
            {
                std::cerr << "FBX2glTF failed with exit code: " << result << std::endl;
                return "";
            }

            if (std::filesystem::exists(gltfPath))
            {
                return gltfPath;
            }
            else
            {
                std::cerr << "Output file not found after conversion." << std::endl;
                return "";
            }
        }
    }
}
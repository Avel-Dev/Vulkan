#pragma once
#include "VulkanHeader.h"
#include "Buffer.h"
#include "Material.h"

namespace CHIKU
{
    struct PipelineKey 
    {
        Buffer::VertexLayoutPreset vertexLayout;
        MaterialLayoutPreset materialLayout;
        uint32_t shaderID;           // ID or hash of shader module(s)
        uint32_t renderPassID;       // Optional: depends on your pipeline structure

        bool operator==(const PipelineKey& other) const {
            return vertexLayout == other.vertexLayout &&
                materialLayout == other.materialLayout &&
                shaderID == other.shaderID &&
                renderPassID == other.renderPassID;
        }
    };
}


// Hash function for unordered_map
namespace std 
{
    template <>
    struct hash<CHIKU::PipelineKey> 
    {
        size_t operator()(const CHIKU::PipelineKey& key) const 
        {
            return std::hash<int>()(static_cast<int>(key.vertexLayout)) ^
                std::hash<int>()(static_cast<int>(key.materialLayout)) ^
                std::hash<uint32_t>()(key.shaderID) ^
                std::hash<uint32_t>()(key.renderPassID);
        }
    };
}

#include "Mesh.h"

namespace CHIKU
{
    VkVertexInputBindingDescription Mesh::GetVertexBufferBindingDescription()
    {
        return VkVertexInputBindingDescription();
    }

    std::vector<VkVertexInputAttributeDescription> Mesh::GetVertexBufferAttributeDescriptions()
    {
        return std::vector<VkVertexInputAttributeDescription>();
        // TODO: insert return statement here
    }
}

#pragma once
#include "VulkanHeader.h"
#include "Material.h"
#include "Mesh.h"
#include <iostream>

namespace CHIKU
{
	class Model
	{
	public:
		~Model();
		void LoadMesh(std::string path);
		const Material& GetMaterial() const { return m_Material; }
		const Mesh& GetMesh() const { return m_Mesh; }
		void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipeline) const;
	private:
		Material m_Material;
		Mesh m_Mesh;
	};
}
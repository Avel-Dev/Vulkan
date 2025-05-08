#pragma once
#include "VulkanHeader.h"
#include "Material.h"
#include "Mesh.h"

namespace CHIKU
{
	class Model
	{
	public:
		void LoadMesh(std::string path);

	private:
		Material m_Material;
		Mesh m_Mesh;
	};
}
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

	private:
		Material m_Material;
		Mesh m_Mesh;
	};
}
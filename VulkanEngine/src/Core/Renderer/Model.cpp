#include "Model.h"
#include <tiny_obj_loader.h>

namespace CHIKU
{
	void Model::LoadMesh(std::string path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (SOURCE_DIR + path).c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		m_Mesh.LoadMesh(attrib, shapes);
	}
}

#include "Model.h"
#include <algorithm>
#include <tiny_obj_loader.h>

namespace CHIKU
{
	Model::~Model()
	{
		m_Mesh.CleanUp();
	}

	void Model::LoadMesh(std::string path)
	{
		m_Material.CreateMaterial(MaterialLayoutPreset::MVPUnlitTextured);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (SOURCE_DIR + path).c_str(), (SOURCE_DIR + "models/").c_str()))
		{
			throw std::runtime_error(warn + err);
		}
		if (!warn.empty()) {
			std::cout << "Warning: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cerr << "Error: " << err << std::endl;
		}

		for (size_t i = 0; i < materials.size(); i++) {
			const tinyobj::material_t& mat = materials[i];
			std::cout << "Material " << i << ": " << mat.name << std::endl;
			std::cout << "  Diffuse Texture: " << mat.diffuse_texname << std::endl;
			std::cout << "  Specular Texture: " << mat.specular_texname << std::endl;
			std::cout << "  Normal Texture: " << mat.normal_texname << std::endl;
			std::cout << "  Ambient Texture: " << mat.ambient_texname << std::endl;
			std::cout << "  Emissive Texture: " << mat.emissive_texname << std::endl;
			std::cout << "  Alpha Texture: " << mat.alpha_texname << std::endl;
			std::cout << "  Bump Texture: " << mat.bump_texname << std::endl;
			std::cout << "  Roughness Texture: " << mat.roughness_texname << std::endl;
			std::cout << "  Metalness Texture: " << mat.metallic_texname << std::endl;
			std::cout << "  Sheen Texture: " << mat.sheen_texname << std::endl;
		}

		m_Mesh.LoadMesh(attrib, shapes);
	}
}

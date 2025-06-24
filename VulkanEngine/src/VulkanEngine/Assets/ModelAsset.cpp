#include "ModelAsset.h"
#include "MeshAsset.h"
#include "AssetManager.h"
#include "Utils/ModelUtils.h"
#include "Utils/BufferUtils.h"
#include <iostream>

namespace CHIKU
{
    void ModelAsset::CreateMeshes(const tinygltf::Model& model)
    {
        std::vector<Utils::GLTFVertexBufferMetaData> info = Utils::GetVertexLayout(model);
        std::vector<std::vector<uint8_t>> data(info.size());

        int n = info.size();
        for (int i = 0; i < n; i++)
        {
            Utils::CreateVertexData(info[i], data[i]);
        }

        for (int i = 0; i < n; i++)
        {
            AssetManager::AddMesh(Utils::ConvertGLTFInfoToVertexInfo(info[i]), data[i]);
        }
    }

    void ModelAsset::CreateMaterials(const tinygltf::Model& model)
    {

    }


	bool ModelAsset::LoadModel(const AssetPath& path)
	{
        ZoneScoped;

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;
		
		AssetPath newPath = Utils::ConvertToGLTF(SOURCE_DIR + path.string());

		bool ok = loader.LoadASCIIFromFile(&model, &err, &warn, newPath.string());
		if (!warn.empty()) std::cout << "Warn: " << warn << "\n";
		if (!err.empty()) std::cerr << "Err: " << err << "\n";

		CreateMeshes(model);

        return ok;
	}
}
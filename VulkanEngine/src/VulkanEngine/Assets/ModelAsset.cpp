#include "ModelAsset.h"
#include "MeshAsset.h"
#include "AssetManager.h"
#include "Utils/ModelUtils.h"
#include "Utils/BufferUtils.h"
#include <tinygltf/tiny_gltf.h>
#include <iostream>

namespace CHIKU
{

    bool IsGLTFFormat(const AssetPath& path)
    {
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext == ".gltf" || ext == ".glb";
    }

    AssetPath ConvertToGLTF(const AssetPath& modelAsset)
    {
        if (IsGLTFFormat(modelAsset)) 
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


        if (std::filesystem::exists(gltfPath)) {
            return gltfPath;
        }
        else {
            std::cerr << "Output file not found after conversion." << std::endl;
            return "";
        }
    }

	bool ModelAsset::LoadModel(const AssetPath& path)
	{
        ZoneScoped;

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;
		
		AssetPath newPath = ConvertToGLTF(SOURCE_DIR + path.string());

		bool ok = loader.LoadASCIIFromFile(&model, &err, &warn, newPath.string());
		if (!warn.empty()) std::cout << "Warn: " << warn << "\n";
		if (!err.empty()) std::cerr << "Err: " << err << "\n";

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

        /*
        std::vector<VertexBuffer> vertexBuffers(data.size());

        for (int i = 0; i < data.size(); i++)
        {
            vertexBuffers[i].CreateVertexBuffer(data[i]);
        }

        for (int i = 0; i < data.size(); i++)
        {
            vertexBuffers[i].CleanUp();
        }*/

        return ok;
	}
}
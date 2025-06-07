#pragma once
#include <string>
#include <filesystem>

namespace CHIKU
{
	using AssetHandle = uint64_t;
	using AssetPath = std::filesystem::path;

	enum class AssetType : int8_t
	{
		None,
		Texture2D,
		TextureCube,
		Model,
		Shader,
		Material,
		Mesh,
		Sound,
	};

	inline AssetType AssetTypeFromString(const std::string& str) {
			
		if (str == "None"		)	return AssetType::None;
		if (str == "Texture2D"	)	return AssetType::Texture2D;
		if (str == "TextureCube")   return AssetType::TextureCube;
		if (str == "Model"		)	return AssetType::Model;
		if (str == "Shader"		)	return AssetType::Shader;
		if (str == "Material"	)	return AssetType::Material;
		if (str == "Mesh"		)	return AssetType::Mesh;
		if (str == "Sound"		)	return AssetType::Sound;
		
		return AssetType::None;
	}

	inline std::string AssetTypeToString(AssetType type) {

		switch (type) 
		{
			case AssetType::None:			return "None";
			case AssetType::Texture2D:		return "Texture2D";
			case AssetType::TextureCube:	return "TextureCube";
			case AssetType::Model:			return "Model";
			case AssetType::Shader:			return "Shader";
			case AssetType::Material:		return "Material";
			case AssetType::Mesh:			return "Mesh";
			case AssetType::Sound:			return "Sound";
			default: return "None";
		}

		return "None";
	}

	class Asset
	{
	public:
		static Asset CreateAsset(AssetHandle handle, AssetType type, const std::string& path);

		static constexpr uint64_t InvalidHandle = std::numeric_limits<uint64_t>::max();

		Asset() : m_Handle(InvalidHandle), m_Type(AssetType::None) {}
		Asset(AssetType type) : m_Type(type), m_Handle(InvalidHandle) {}
		Asset(AssetHandle handle) : m_Handle(handle), m_Type(AssetType::None) {}
		Asset(AssetHandle handle, AssetType type) : m_Handle(handle), m_Type(type) {}
		Asset(AssetHandle handle, AssetType type, AssetPath path) : m_Handle(handle), m_Type(type), m_Path(path) {}

		virtual AssetHandle GetHandle() const final { return m_Handle; }
		virtual AssetType GetType() const final { return m_Type; }

	public:
		//This is only to know if the this asset was loaded with data
		//to check if the handle stored in this class is valid use the AssetManager
		operator bool() const 
		{ 
			return m_Handle != InvalidHandle || m_Type != AssetType::None;
		} 

		AssetType m_Type;
		AssetHandle m_Handle;
		AssetPath m_Path;
	};
}
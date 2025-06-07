#pragma once
#include "Asset.h"

namespace CHIKU
{
	class ModelAsset : public Asset
	{
	public:
		ModelAsset() : Asset() {}
		ModelAsset(AssetType type) : Asset(type) {}
		ModelAsset(AssetHandle handle) : Asset(handle) {}
		ModelAsset(AssetHandle handle, AssetType type) : Asset(handle,type) {}
		ModelAsset(AssetHandle handle, AssetType type, AssetPath path) : Asset(handle,type,path) {}


	};

	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset() : Asset() {}
		MaterialAsset(AssetType type) : Asset(type) {}
		MaterialAsset(AssetHandle handle) : Asset(handle) {}
		MaterialAsset(AssetHandle handle, AssetType type) : Asset(handle, type) {}
		MaterialAsset(AssetHandle handle, AssetType type, AssetPath path) : Asset(handle, type, path) {}
	};

	class MeshAsset : public Asset
	{
	public:
		MeshAsset() : Asset() {}
		MeshAsset(AssetType type) : Asset(type) {}
		MeshAsset(AssetHandle handle) : Asset(handle) {}
		MeshAsset(AssetHandle handle, AssetType type) : Asset(handle, type) {}
		MeshAsset(AssetHandle handle, AssetType type, AssetPath path) : Asset(handle, type, path) {}
	};

	class SpriteAsset : public Asset
	{
	public:
		SpriteAsset() : Asset() {}
		SpriteAsset(AssetType type) : Asset(type) {}
		SpriteAsset(AssetHandle handle) : Asset(handle) {}
		SpriteAsset(AssetHandle handle, AssetType type) : Asset(handle, type) {}
		SpriteAsset(AssetHandle handle, AssetType type, AssetPath path) : Asset(handle, type, path) {}
	};

	class ShaderAsset : public Asset
	{
	public:
		ShaderAsset() : Asset() {}
		ShaderAsset(AssetType type) : Asset(type) {}
		ShaderAsset(AssetHandle handle) : Asset(handle) {}
		ShaderAsset(AssetHandle handle, AssetType type) : Asset(handle, type) {}
		ShaderAsset(AssetHandle handle, AssetType type, AssetPath path) : Asset(handle, type, path) {}
	};

	class SoundAsset : public Asset
	{
	public:
		SoundAsset() : Asset() {}
		SoundAsset(AssetType type) : Asset(type) {}
		SoundAsset(AssetHandle handle) : Asset(handle) {}
		SoundAsset(AssetHandle handle, AssetType type) : Asset(handle, type) {}
		SoundAsset(AssetHandle handle, AssetType type, AssetPath path) : Asset(handle, type, path) {}
	};
}
#include "Precompiled.h"
#include "ModelManager.h"
#include "ModeliO.h"

using namespace SpringEngine;
using namespace SpringEngine::Graphics;

namespace
{
	std::unique_ptr<ModelManager> sModelManager;
}
void ModelManager::StaticInitialize()
{
	ASSERT(sModelManager == nullptr, "ModelManager is already initialized");
	sModelManager = std::make_unique<ModelManager>();

}

void ModelManager::StaticTerminate()
{
	sModelManager.reset();
}

ModelManager* ModelManager::Get()
{
	ASSERT(sModelManager != nullptr, "Modelmanager: was not initialized");

	return sModelManager.get();
}

ModelId ModelManager::GetModelId(const std::filesystem::path& filepath)
{

	return std::filesystem::hash_value(filepath);
}

ModelId ModelManager::LoadModel(const std::filesystem::path& filepath)
{
	const ModelId modelId = GetModelId(filepath);
	auto [iter, success] = mInventory.insert({ modelId,nullptr });
	if (success)
	{
		auto& modelPtr = iter->second;
		modelPtr = std::make_unique<Model>();
		ModelIO::LoadModel(filepath, *modelPtr);
		ModelIO::LoadMaterial(filepath, *modelPtr);
	}
	return modelId;
}

const Model* ModelManager::GetModel(ModelId id)
{
	auto model = mInventory.find(id);
	if (model != mInventory.end())
	{
		return model->second.get();
	}
	return nullptr;
}

#pragma once

#include <filesystem>
#include <unordered_map>

#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "TextureManager.h"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "assimp/texture.h"

class SceneLoader {
private:
public:
	static Scene Load(
		std::filesystem::path& path, ResourceManager& resourceManager
	);
};

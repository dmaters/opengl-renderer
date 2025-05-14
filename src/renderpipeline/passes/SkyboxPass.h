#pragma once

#include <optional>

#include "ResourceManager.h"
#include "Resources.h"
#include "renderpipeline/FullscreenPass.h"

class SkyboxPass {
private:
	std::optional<FullscreenPass> m_pass;

public:
	SkyboxPass(TextureHandle skybox, ResourceManager& resourceManager);
	void render(ResourceManager& resourceManager);
};
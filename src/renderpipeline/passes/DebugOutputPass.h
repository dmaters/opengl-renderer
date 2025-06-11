#pragma once

#include "ResourceManager.h"
#include "Resources.h"
#include "renderpipeline/FullscreenPass.h"
class DebugOutputPass {
	std::optional<FullscreenPass> m_pass;

public:
	DebugOutputPass(ResourceManager& resourceManager);
	void setTexture(TextureHandle handle, ResourceManager& resourceManager);
	void render(ResourceManager& resourceManager);
};
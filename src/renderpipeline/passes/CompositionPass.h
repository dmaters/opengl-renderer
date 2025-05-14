#pragma once

#include <optional>

#include "ResourceManager.h"
#include "renderpipeline/FullscreenPass.h"

class CompositionPass {
private:
	std::optional<FullscreenPass> m_pass;

public:
	CompositionPass(
		TextureHandle finalRenderTarget, ResourceManager& resourceManager
	);

	void render(ResourceManager& resourceManager);
};
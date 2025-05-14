#pragma once

#include <optional>

#include "ResourceManager.h"
#include "Resources.h"
#include "renderpipeline/FrameBuffer.h"
#include "renderpipeline/FullscreenPass.h"

struct GBufferOutput;

class LightingPass {
private:
	MaterialHandle m_lightingMaterial;
	std::optional<FullscreenPass> m_pass;
	FrameBuffer m_finalFB;

public:
	LightingPass(
		GBufferOutput gbufferData,
		TextureHandle irradianceOutput,
		ResourceManager& resourceManager
	);
	void render(glm::ivec2 resolution, ResourceManager& resourceManager);

	TextureHandle getResult() const {
		return m_finalFB.getAttachment(FrameBufferAttachment::COLOR0);
	}
};
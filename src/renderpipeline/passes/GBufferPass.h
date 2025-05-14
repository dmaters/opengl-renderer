#pragma once

#include <optional>

#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "renderpipeline/BasePass.h"
#include "renderpipeline/FrameBuffer.h"

struct GBufferOutput {
	TextureHandle albedo;
	TextureHandle normal;
	TextureHandle worldPosition;
	TextureHandle metallicRoughness;

	TextureHandle depth;
};

class GBufferPass {
private:
	FrameBuffer m_gbufferFB;
	std::optional<BasePass> m_pass;

public:
	GBufferPass(ResourceManager& resourceManager);

	void render(
		glm::ivec2 resolution, Scene& scene, ResourceManager& m_resourceManager
	);

	GBufferOutput getOutput();
};
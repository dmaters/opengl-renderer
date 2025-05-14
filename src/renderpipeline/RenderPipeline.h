#pragma once

#include <glad/glad.h>

#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <optional>

#include "ComputeShader.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "passes/CompositionPass.h"
#include "passes/GBufferPass.h"
#include "passes/IrradiancePass.h"
#include "passes/LightingPass.h"
#include "passes/ShadowPass.h"
#include "passes/SkyboxPass.h"

class RenderPipeline {
public:
	struct RenderSpecifications;

private:
	bool m_shadowmapsGenerated = false;

	std::optional<ShadowPass> m_shadowPass;
	std::optional<IrradiancePass> m_irradiancePass;
	std::optional<GBufferPass> m_gbufferPass;
	std::optional<LightingPass> m_lightingPass;
	std::optional<SkyboxPass> m_skyboxPass;

	std::optional<CompositionPass> m_compositionPass;

	std::shared_ptr<ResourceManager> m_resourceManager;

public:
	RenderPipeline(std::shared_ptr<ResourceManager> resourceManager);

	void render(RenderSpecifications& specs);
};

struct RenderPipeline::RenderSpecifications {
	Scene& scene;
	glm::ivec2 resolution;
};

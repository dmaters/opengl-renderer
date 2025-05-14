
#include "RenderPipeline.h"

#include <glad/glad.h>

#include <functional>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "ComputeShader.h"
#include "FrameBuffer.h"
#include "Frustum.hpp"
#include "Light.h"
#include "Material.h"
#include "Node.h"
#include "Primitive.h"
#include "Program.h"
#include "RenderPass.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "TextureManager.h"
#include "passes/CompositionPass.h"
#include "passes/GBufferPass.h"
#include "passes/IrradiancePass.h"
#include "passes/LightingPass.h"
#include "passes/ShadowPass.h"

RenderPipeline::RenderPipeline(std::shared_ptr<ResourceManager> resourceManager
) :
	m_resourceManager(resourceManager) {
	TextureManager& textureManager = resourceManager->getTextureManager();

	TextureHandle skybox = m_resourceManager->getTextureManager().loadTexture(
		"resources/textures/skybox.hdr"
	);

	m_shadowPass = ShadowPass(*resourceManager);
	m_irradiancePass = IrradiancePass(skybox, *resourceManager);
	m_gbufferPass = GBufferPass(*resourceManager);
	m_lightingPass = LightingPass(
		m_gbufferPass->getOutput(),
		m_irradiancePass->computeIrradiance(),
		*resourceManager
	);
	m_skyboxPass = SkyboxPass(skybox, *resourceManager);

	m_compositionPass =
		CompositionPass(m_lightingPass->getResult(), *resourceManager);
}

void RenderPipeline::render(RenderSpecifications& specs) {
	if (!m_shadowmapsGenerated) {
		m_shadowPass.value().render(specs.scene, *m_resourceManager);
		m_shadowmapsGenerated = true;
	}

	m_gbufferPass.value().render(
		specs.resolution, specs.scene, *m_resourceManager
	);
	m_lightingPass->render(specs.resolution, *m_resourceManager);
	m_skyboxPass->render(*m_resourceManager);
	m_compositionPass->render(*m_resourceManager);
}


#include "RenderPipeline.h"

#include <glad/glad.h>

#include <glm/ext/matrix_transform.hpp>
#include <memory>

#include "FullscreenPass.h"
#include "Material.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "TextureManager.h"
#include "passes/CompositionPass.h"
#include "passes/DebugOutputPass.h"
#include "passes/GBufferPass.h"
#include "passes/IBLOcclusionPass.h"
#include "passes/IrradiancePass.h"
#include "passes/LightingPass.h"
#include "passes/ShadowPass.h"

RenderPipeline::RenderPipeline(std::shared_ptr<ResourceManager> resourceManager
) :
	m_resourceManager(resourceManager) {
	TextureManager& textureManager = resourceManager->getTextureManager();

	TextureHandle skybox =
		textureManager.loadTexture("resources/textures/skybox.hdr");

	m_shadowPass = ShadowPass(*resourceManager);
	m_irradiancePass = IrradiancePass(skybox, *resourceManager);
	m_gbufferPass = GBufferPass(*resourceManager);

	m_iblOcclusionPass =
		IBLOcclusionPass(*resourceManager, m_gbufferPass->getOutput());

	m_lightingPass = LightingPass(
		m_gbufferPass->getOutput(),
		m_irradiancePass->getDiffuseMap(),
		m_irradiancePass->getSpecularMap(),
		m_iblOcclusionPass->getTexture(),
		*resourceManager
	);
	m_skyboxPass = SkyboxPass(skybox, *resourceManager);

	m_compositionPass =
		CompositionPass(m_lightingPass->getResult(), *resourceManager);

	m_debugPass = DebugOutputPass(*resourceManager);
	m_debugPass->setTexture(m_iblOcclusionPass->getTexture(), *resourceManager);
}

void RenderPipeline::render(RenderSpecifications& specs) {
	if (!m_shadowmapsGenerated) {
		m_shadowPass->render(specs.scene, *m_resourceManager);
		m_shadowmapsGenerated = true;
	}
	if (!m_iblGenerated) {
		m_irradiancePass->computeIrradiance(*m_resourceManager);
		m_iblGenerated = true;
	}

	m_gbufferPass->render(specs.resolution, specs.scene, *m_resourceManager);
	m_iblOcclusionPass->render(
		specs.resolution, specs.scene, *m_resourceManager
	);
	m_lightingPass->render(specs.resolution, *m_resourceManager);
	m_skyboxPass->render(*m_resourceManager);
	m_compositionPass->render(*m_resourceManager);

	// m_debugPass->render(*m_resourceManager);
}

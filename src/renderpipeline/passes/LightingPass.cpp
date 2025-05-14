#include "LightingPass.h"

#include "GBufferPass.h"
#include "ResourceManager.h"
#include "Resources.h"

LightingPass::LightingPass(
	GBufferOutput gbufferData,
	TextureHandle irradianceOutput,
	ResourceManager& resourceManager
) {
	ProgramHandle lightingProgram = resourceManager.registerProgram({
		.vertex = "resources/shaders/quad.vert",
		.fragment = "resources/shaders/lighting_pbr.frag",
	});
	m_lightingMaterial =
		resourceManager.registerMaterial(Material(lightingProgram));

	Material& material = resourceManager.getMaterial(m_lightingMaterial);
	material.setUniform("_albedo", gbufferData.albedo);
	material.setUniform("_normal", gbufferData.normal);
	material.setUniform("_world_position", gbufferData.worldPosition);
	material.setUniform("_roughness_metallic", gbufferData.metallicRoughness);
	material.setUniform("irradiance_map", irradianceOutput);
	material.setUniform("LightsData", UBOHandle::LIGHTS);

	m_pass = FullscreenPass(m_lightingMaterial);

	m_finalFB = FrameBuffer::getGeneralRenderPassFB(
		resourceManager.getTextureManager(), glm::uvec2(1, 1)
	);

	m_finalFB.setAttachment(
		FrameBufferAttachment::DEPTH,
		gbufferData.depth,
		resourceManager.getTextureManager()
	);
}

void LightingPass::render(
	glm::ivec2 resolution, ResourceManager& resourceManager
) {
	m_finalFB.setResolution(resolution, resourceManager.getTextureManager());
	m_finalFB.bind();

	m_pass.value().render(resourceManager);
}
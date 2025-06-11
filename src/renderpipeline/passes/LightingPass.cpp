#include "LightingPass.h"

#include "GBufferPass.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "glad/glad.h"

LightingPass::LightingPass(
	GBufferOutput gbufferData,
	TextureHandle diffuseIrradiance,
	TextureHandle specularIrradiance,
	TextureHandle occlusionMask,
	ResourceManager& resourceManager
) {
	ProgramHandle lightingProgram = resourceManager.registerProgram({
		.vertex = "resources/shaders/quad.vert",
		.fragment = "resources/shaders/lighting_direct.frag",
	});
	MaterialHandle directMaterialhandle =
		resourceManager.registerMaterial(Material(lightingProgram));

	Material& directLightingMaterial =
		resourceManager.getMaterial(directMaterialhandle);
	directLightingMaterial.setUniform("_albedo", gbufferData.albedo);
	directLightingMaterial.setUniform("_normal", gbufferData.normal);
	directLightingMaterial.setUniform(
		"_world_position", gbufferData.worldPosition
	);
	directLightingMaterial.setUniform(
		"_metallic_roughness", gbufferData.metallicRoughness
	);
	directLightingMaterial.setUniform("LightsData", UBOHandle::LIGHTS);

	m_directLightingPass = FullscreenPass(directMaterialhandle);

	TextureHandle brdfLUT = resourceManager.getTextureManager().loadTexture(
		"resources/textures/ibl_brdf_lut.png"
	);

	ProgramHandle iblPass = resourceManager.registerProgram({
		.vertex = "resources/shaders/quad.vert",
		.fragment = "resources/shaders/lighting_ibl.frag",
	});
	MaterialHandle iblMaterialHandle =
		resourceManager.registerMaterial(Material(iblPass));

	Material& material = resourceManager.getMaterial(iblMaterialHandle);
	material.setUniform("_albedo", gbufferData.albedo);
	material.setUniform("_normal", gbufferData.normal);
	material.setUniform("_world_position", gbufferData.worldPosition);
	material.setUniform("_metallic_roughness", gbufferData.metallicRoughness);
	material.setUniform("irradiance_specular", specularIrradiance);
	material.setUniform("irradiance_diffuse", diffuseIrradiance);
	material.setUniform("brdf_lut", brdfLUT);
	material.setUniform("occlusion_mask", occlusionMask);

	m_directLightingPass = FullscreenPass(directMaterialhandle);
	m_iblPass = FullscreenPass(iblMaterialHandle);
	m_finalFB = FrameBuffer::getGeneralRenderPassFB(
		resourceManager.getTextureManager(), glm::uvec2(1, 1)
	);

	m_finalFB.setAttachment(
		FrameBufferAttachment::DEPTH_STENCIL,
		gbufferData.depth,
		resourceManager.getTextureManager()
	);
}

void LightingPass::render(
	glm::ivec2 resolution, ResourceManager& resourceManager
) {
	m_finalFB.setResolution(resolution, resourceManager.getTextureManager());
	m_finalFB.bind();

	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glBlendFunc(GL_SRC_ALPHA, 1);

	m_directLightingPass->render(resourceManager);
	m_iblPass->render(resourceManager);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

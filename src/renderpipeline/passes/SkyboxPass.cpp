#include "SkyboxPass.h"

#include "Material.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "renderpipeline/FullscreenPass.h"

SkyboxPass::SkyboxPass(TextureHandle skybox, ResourceManager& resourceManager) {
	ProgramHandle skyboxProgram = resourceManager.registerProgram({
		.vertex = "resources/shaders/skybox.vert",
		.fragment = "resources/shaders/skybox.frag",
	});

	MaterialHandle handle =
		resourceManager.registerMaterial(Material(skyboxProgram));

	Material& material = resourceManager.getMaterial(handle);

	material.setUniform("projection_view", UBOHandle::PROJECTION_VIEW);
	material.setUniform("skybox", skybox);

	m_pass = FullscreenPass(handle, true);
}

void SkyboxPass::render(ResourceManager& resourceManager) {
	m_pass.value().render(resourceManager);
}
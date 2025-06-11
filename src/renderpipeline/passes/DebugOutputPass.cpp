#include "DebugOutputPass.h"

#include "ResourceManager.h"
#include "Resources.h"
#include "glad/glad.h"

DebugOutputPass::DebugOutputPass(ResourceManager& resourceManager) {
	ProgramHandle debugProgramHandle = resourceManager.registerProgram({
		.vertex = "resources/shaders/quad.vert",
		.fragment = "resources/shaders/output_texture.frag",
	});
	MaterialHandle debugMaterialHandle =
		resourceManager.registerMaterial(Material(debugProgramHandle));

	m_pass = FullscreenPass(debugMaterialHandle);
}

void DebugOutputPass::setTexture(
	TextureHandle texture, ResourceManager& resourceManager
) {
	Material& material = resourceManager.getMaterial(m_pass->getMaterial());

	material.setUniform("_texture", texture);
}

void DebugOutputPass::render(ResourceManager& resourceManager) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_pass->render(resourceManager);
}
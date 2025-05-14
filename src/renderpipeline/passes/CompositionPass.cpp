#include "CompositionPass.h"

#include "ResourceManager.h"
#include "Resources.h"
#include "renderpipeline/FullscreenPass.h"

CompositionPass::CompositionPass(
	TextureHandle finalRenderTarget, ResourceManager& resourceManager
) {
	ProgramHandle compositionProgram = resourceManager.registerProgram({
		.vertex = "resources/shaders/quad.vert",
		.fragment = "resources/shaders/comp.frag",
	});
	MaterialHandle handle =
		resourceManager.registerMaterial(Material(compositionProgram));

	m_pass = FullscreenPass(handle);

	Material& material = resourceManager.getMaterial(handle);
	material.setUniform("attachment", finalRenderTarget);
}

void CompositionPass::render(ResourceManager& resourceManager) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pass.value().render(resourceManager);
}
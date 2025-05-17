#include "FullscreenPass.h"

#include "glad/glad.h"

void FullscreenPass::render(ResourceManager& resourceManager) {
	Material& material = resourceManager.getMaterial(m_material);
	material.bind(resourceManager);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glStencilMask(0x00);
	glBindVertexArray(resourceManager.getQuadVAO());
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}
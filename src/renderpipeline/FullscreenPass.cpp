#include "FullscreenPass.h"

void FullscreenPass::render(ResourceManager& resourceManager) {
	Material& material = resourceManager.getMaterial(m_material);
	material.bind(resourceManager);
	if (!m_depthEnabled) {
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}
	glBindVertexArray(resourceManager.getQuadVAO());
	glDrawArrays(GL_TRIANGLES, 0, 3);
	if (!m_depthEnabled) {
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}
}
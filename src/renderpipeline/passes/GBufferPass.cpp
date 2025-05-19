#include "GBufferPass.h"

#include "ResourceManager.h"
#include "Scene.h"
#include "glad/glad.h"
#include "renderpipeline/BasePass.h"

GBufferPass::GBufferPass(ResourceManager& resourceManager) {
	m_gbufferFB = FrameBuffer::getGBufferPassFB(
		resourceManager.getTextureManager(), glm::uvec2(800, 600)
	);
	m_pass = BasePass(MaterialHandle::DEFAULT);
}

void GBufferPass::render(
	glm::ivec2 resolution, Scene& scene, ResourceManager& resourceManager
) {
	m_gbufferFB.setResolution(resolution, resourceManager.getTextureManager());
	m_gbufferFB.bind();

	Frustum frustum = scene.getCamera().getFrustum(
		glm::vec2(0.1, 10000), (float)resolution.x / resolution.y
	);

	std::vector<std::reference_wrapper<Primitive>> primitives =
		scene.getPrimitives([frustum](Primitive& primitive) {
			return frustum.isSphereInFrustum(
				primitive.getPosition(), primitive.getSize()
			);
		});
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	m_pass.value().render(primitives, resourceManager);
	glStencilMask(0x00);
}

GBufferOutput GBufferPass::getOutput() {
	return {
		.albedo = m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR0),
		.normal = m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR1),
		.worldPosition =
			m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR2),
		.metallicRoughness =
			m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR3),
		.depth =
			m_gbufferFB.getAttachment(FrameBufferAttachment::DEPTH_STENCIL),

	};
}
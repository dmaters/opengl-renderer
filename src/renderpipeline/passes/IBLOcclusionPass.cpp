#include "IBLOcclusionPass.h"

#include <functional>
#include <vector>

#include "Material.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "renderpipeline/BasePass.h"
#include "renderpipeline/FrameBuffer.h"
#include "renderpipeline/passes/GBufferPass.h"

IBLOcclusionPass::IBLOcclusionPass(
	ResourceManager& resourceManager, GBufferOutput gbufferOutput
) :
	m_maskPass("resources/shaders/ibl_occlusion.comp") {
	ProgramHandle occlusionProgram = resourceManager.registerProgram({
		.vertex = "resources/shaders/base.vert",
		.fragment = "resources/shaders/dummy.frag",
	});

	MaterialHandle occlusionMaterial =
		resourceManager.registerMaterial(Material(occlusionProgram));
	m_occlusionPass = BasePass(occlusionMaterial);

	m_occlusionBuffer = FrameBuffer::getDepthOnlyFB(
		resourceManager.getTextureManager(), glm::ivec2(512, 512)
	);

	m_maskBuffer = FrameBuffer::getGeneralRenderPassFB(
		resourceManager.getTextureManager(), glm::ivec2(800, 600)
	);

	m_maskPass.setUniform(
		"_normal",
		resourceManager.getTextureManager().getTexture(gbufferOutput.normal)
	);
	m_maskPass.setUniform(
		"_world_position",
		resourceManager.getTextureManager().getTexture(
			gbufferOutput.worldPosition
		)
	);
	m_maskPass.setUniform(
		"_depth",
		resourceManager.getTextureManager().getTexture(gbufferOutput.depth)
	);

	m_maskPass.setUniform(
		"_occlusion_map",
		resourceManager.getTextureManager().getTexture(
			m_occlusionBuffer.getAttachment(FrameBufferAttachment::DEPTH)
		)
	);

	ResourceManager::UBOData data =
		resourceManager.getUBO(UBOHandle::PROJECTION_VIEW);
	m_maskPass.setUBO("projection_view", data.id, data.binding);
}

void IBLOcclusionPass::render(
	glm::ivec2 resolution, Scene& scene, ResourceManager& resourceManager
) {
	if (!m_occlusionGenerated) {
		float size = scene.getSize();
		glm::mat4 proj = glm::ortho(-size, size, -size, size, 0.1f, size * 2);

		glm::mat4 view = glm::mat4(
			glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(0, -1, 0, 0),
			glm::vec4(0, 0, -size, 1)
		);

		Material& occlusionMaterial =
			resourceManager.getMaterial(m_occlusionPass->getMaterial());
		occlusionMaterial.setUniform("projection", proj);
		occlusionMaterial.setUniform("view", view);

		m_maskPass.setUniform("projection", proj);
		m_maskPass.setUniform("view", view);

		m_projection = proj;
		m_view = view;

		m_occlusionBuffer.bind();
		std::vector<std::reference_wrapper<Primitive>> primitives =
			scene.getPrimitives([size](Primitive& primitive) {
				return primitive.getSize() > size / 10;
			});

		m_occlusionPass->render(primitives, resourceManager);
		m_occlusionGenerated = true;
	}

	m_maskBuffer.setResolution(
		glm::ivec2(resolution.x, resolution.y),
		resourceManager.getTextureManager()
	);
	Texture& result = resourceManager.getTextureManager().getTexture(
		m_maskBuffer.getAttachment(FrameBufferAttachment::COLOR0)
	);
	if (resolution != m_resolution) {
		int levels = 1 + floor(log2(glm::max(resolution.x, resolution.y)));
		glTextureStorage2D(
			result.textureID, levels, result.format, resolution.x, resolution.y
		);
		m_resolution = resolution;
	}

	m_maskPass.setUniform("_occlusion_mask", result);

	m_maskBuffer.bind();

	m_maskPass.dispatch(glm::ivec3(resolution.x, resolution.y, 1));

	glGenerateTextureMipmap(result.textureID);
}

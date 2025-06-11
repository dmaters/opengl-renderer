#pragma once

#include "ComputeShader.h"
#include "GBufferPass.h"
#include "ResourceManager.h"
#include "renderpipeline/BasePass.h"
#include "renderpipeline/FrameBuffer.h"

class IBLOcclusionPass {
private:
	std::optional<BasePass> m_occlusionPass;
	ComputeShader m_maskPass;

	FrameBuffer m_occlusionBuffer;
	FrameBuffer m_maskBuffer;

	bool m_occlusionGenerated = false;

	glm::mat4 m_projection;
	glm::mat4 m_view;

	glm::ivec2 m_resolution = glm::ivec2(0);

public:
	IBLOcclusionPass(
		ResourceManager& resourceManager, GBufferOutput gbufferOutput
	);

	void render(
		glm::ivec2 resolution, Scene& scene, ResourceManager& resourceManager
	);

	TextureHandle getTexture() {
		return m_maskBuffer.getAttachment(FrameBufferAttachment::COLOR0);
	}
};
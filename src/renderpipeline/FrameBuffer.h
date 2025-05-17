#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <unordered_map>

#include "Resources.h"
#include "TextureManager.h"
#include "glm/fwd.hpp"

enum class FrameBufferAttachment {
	COLOR0,
	COLOR1,
	COLOR2,
	COLOR3,
	DEPTH,
	DEPTH_STENCIL
};

class FrameBuffer {
private:
	GLuint m_framebuffer;

	std::unordered_map<FrameBufferAttachment, TextureHandle> m_attachments;
	bool m_clearOnBind;
	glm::ivec2 m_resolution;

	GLbitfield m_clearMask;

protected:
public:
	static FrameBuffer getShadowMapFB(
		TextureManager& textureManager, glm::ivec2 resolution
	);
	static FrameBuffer getGBufferPassFB(
		TextureManager& textureManager, glm::ivec2 resolution
	);
	static FrameBuffer getGeneralRenderPassFB(
		TextureManager& textureManager, glm::ivec2 resolution
	);

	TextureHandle getAttachment(FrameBufferAttachment attachment) const {
		if (m_attachments.find(attachment) == m_attachments.end())
			return TextureHandle::UNASSIGNED;

		return m_attachments.at(attachment);
	};
	void bind();
	void setAttachment(
		FrameBufferAttachment attachment,
		TextureHandle handle,
		TextureManager& textureManager
	);

	GLuint getID() const { return m_framebuffer; };

	void setResolution(glm::ivec2 resolution, TextureManager& textureManager);
};
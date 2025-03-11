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
	STENCIL
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
		TextureManager& textureManager, glm::ivec2 resolution, bool clear
	);
	static FrameBuffer getForwardFB(
		TextureManager& textureManager, glm::ivec2 resolution, bool clear
	);

	TextureHandle getAttachment(FrameBufferAttachment attachment) const {
		if (m_attachments.find(attachment) == m_attachments.end())
			return TextureHandle::UNASSIGNED;

		return m_attachments.at(attachment);
	};
	void bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		glViewport(0, 0, m_resolution.x, m_resolution.y);

		if (m_clearOnBind) glClear(m_clearMask);
	}

	void setAttachment(
		FrameBufferAttachment attachment,
		TextureHandle handle,
		TextureManager& textureManager
	);

	GLuint getID() const { return m_framebuffer; };

	void setResolution(glm::ivec2 resolution, TextureManager& textureManager);
};
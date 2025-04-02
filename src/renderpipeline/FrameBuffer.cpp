#include "FrameBuffer.h"

#include <cassert>

#include "Resources.h"
#include "TextureManager.h"
#include "glad/glad.h"

FrameBuffer FrameBuffer::getShadowMapFB(
	TextureManager& textureManager, glm::ivec2 resolution
) {
	FrameBuffer fb;
	fb.m_clearOnBind = true;
	fb.m_resolution = resolution;
	glCreateFramebuffers(1, &fb.m_framebuffer);
	TextureManager::TextureSpecification colorSpecs = {
		.definition = {
			.format = GL_RGB10_A2,
			.width = resolution.x,
			.height = resolution.y,
		},

	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR0,
		textureManager.createTexture(colorSpecs),
		textureManager
	);
	fb.m_clearMask = GL_DEPTH_BUFFER_BIT;

	return fb;
}

FrameBuffer FrameBuffer::getForwardFB(
	TextureManager& textureManager, glm::ivec2 resolution
) {
	FrameBuffer fb;
	fb.m_clearOnBind = true;
	fb.m_resolution = resolution;
	glCreateFramebuffers(1, &fb.m_framebuffer);

	TextureManager::TextureSpecification colorSpecs = {
		.definition = {
			.format = GL_RGB10_A2,
			.width = resolution.x,
			.height = resolution.y,
		},

	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR0,
		textureManager.createTexture(colorSpecs),
		textureManager
	);

	TextureManager::TextureSpecification depthSpecs = {
		.definition = {
			.format = GL_DEPTH_COMPONENT24,
			.width = resolution.x,
			.height = resolution.y,
		},

	};
	fb.setAttachment(
		FrameBufferAttachment::DEPTH,
		textureManager.createTexture(depthSpecs),
		textureManager
	);

	fb.m_clearMask = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;

	return fb;
}
FrameBuffer FrameBuffer::getIrradianceFB(
	TextureManager& textureManager, glm::ivec2 resolution
) {
	FrameBuffer fb;
	fb.m_clearOnBind = true;
	fb.m_resolution = resolution;
	glCreateFramebuffers(1, &fb.m_framebuffer);

	TextureManager::TextureSpecification colorSpecs = {
		.definition = {
			.format = GL_RGB8,
			.type = GL_TEXTURE_CUBE_MAP,
			.width = resolution.x,
			.height = resolution.y,
			.depth = 6,
		},

	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR0,
		textureManager.createTexture(colorSpecs),
		textureManager
	);
	return fb;
}

void FrameBuffer::setAttachment(
	FrameBufferAttachment attachment,
	TextureHandle handle,
	TextureManager& textureManager
) {
	m_attachments[attachment] = handle;

	GLenum attachmentType;

	switch (attachment) {
		case FrameBufferAttachment::COLOR0:
			attachmentType = GL_COLOR_ATTACHMENT0;
			break;
		case FrameBufferAttachment::COLOR1:
			attachmentType = GL_COLOR_ATTACHMENT1;
			break;
		case FrameBufferAttachment::COLOR2:
			attachmentType = GL_COLOR_ATTACHMENT2;
			break;
		case FrameBufferAttachment::COLOR3:
			attachmentType = GL_COLOR_ATTACHMENT3;
			break;
		case FrameBufferAttachment::DEPTH:
			attachmentType = GL_DEPTH_ATTACHMENT;
			break;
		case FrameBufferAttachment::STENCIL:
			attachmentType = GL_STENCIL_ATTACHMENT;
			break;
	}

	glNamedFramebufferTexture(
		m_framebuffer,
		attachmentType,
		textureManager.getTexture(handle).textureID,
		0
	);
}

void FrameBuffer::setResolution(
	glm::ivec2 resolution, TextureManager& textureManager
) {
	if (resolution == m_resolution) return;
	for (auto& [type, handle] : m_attachments) {
		Texture& texture = textureManager.getTexture(handle);
		texture.width = resolution.x;
		texture.height = resolution.y;

		TextureManager::TextureSpecification specs { .definition = texture };
		textureManager.createTexture(specs, handle);

		setAttachment(type, handle, textureManager);
		m_resolution = resolution;
	}
}

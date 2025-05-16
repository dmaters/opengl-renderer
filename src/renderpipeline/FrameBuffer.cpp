#include "FrameBuffer.h"

#include <cassert>

#include "Resources.h"
#include "TextureManager.h"
#include "glad/glad.h"

constexpr GLenum attachmentTypeToEnum(FrameBufferAttachment attachment) {
	switch (attachment) {
		case FrameBufferAttachment::COLOR0:
			return GL_COLOR_ATTACHMENT0;
			break;
		case FrameBufferAttachment::COLOR1:
			return GL_COLOR_ATTACHMENT1;
			break;
		case FrameBufferAttachment::COLOR2:
			return GL_COLOR_ATTACHMENT2;
			break;
		case FrameBufferAttachment::COLOR3:
			return GL_COLOR_ATTACHMENT3;
			break;
		case FrameBufferAttachment::DEPTH:
			return GL_DEPTH_ATTACHMENT;
			break;
		case FrameBufferAttachment::STENCIL:
			return GL_STENCIL_ATTACHMENT;
			break;
	}
}

FrameBuffer FrameBuffer::getShadowMapFB(
	TextureManager& textureManager, glm::ivec2 resolution
) {
	FrameBuffer fb;
	fb.m_clearOnBind = true;
	fb.m_resolution = resolution;
	glCreateFramebuffers(1, &fb.m_framebuffer);
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

	fb.m_clearMask = GL_DEPTH_BUFFER_BIT;

	return fb;
}

FrameBuffer FrameBuffer::getGBufferPassFB(
	TextureManager& textureManager, glm::ivec2 resolution
) {
	FrameBuffer fb;
	fb.m_clearOnBind = true;
	fb.m_resolution = resolution;
	fb.m_clearMask = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;

	glCreateFramebuffers(1, &fb.m_framebuffer);

	TextureManager::TextureSpecification albedo = {
		.definition = {
			.format = GL_RGB16F,
			.width = resolution.x,
			.height = resolution.y,
		},
	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR0,
		textureManager.createTexture(albedo),
		textureManager
	);
	TextureManager::TextureSpecification worldSpace = {
		.definition = {
			.format = GL_RGB16F,
			.width = resolution.x,
			.height = resolution.y,
		},
	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR1,
		textureManager.createTexture(worldSpace),
		textureManager
	);
	TextureManager::TextureSpecification normal = {
		.definition = {
			.format = GL_RGB16F,
			.width = resolution.x,
			.height = resolution.y,
		},
	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR2,
		textureManager.createTexture(normal),
		textureManager
	);
	TextureManager::TextureSpecification metallicRoughness = {
		.definition = {
			.format = GL_RG16F,
			.width = resolution.x,
			.height = resolution.y,
		},
	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR3,
		textureManager.createTexture(metallicRoughness),
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

	return fb;
}

FrameBuffer FrameBuffer::getGeneralRenderPassFB(
	TextureManager& textureManager, glm::ivec2 resolution
) {
	FrameBuffer fb;
	fb.m_clearOnBind = true;
	fb.m_resolution = resolution;
	fb.m_clearMask = GL_COLOR_BUFFER_BIT;
	glCreateFramebuffers(1, &fb.m_framebuffer);

	TextureManager::TextureSpecification color = {
		.definition = {
			.format = GL_RGBA16F,
			.width = resolution.x,
			.height = resolution.y,
		},
	};

	fb.setAttachment(
		FrameBufferAttachment::COLOR0,
		textureManager.createTexture(color),
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

	glNamedFramebufferTexture(
		m_framebuffer,
		attachmentTypeToEnum(attachment),
		textureManager.getTexture(handle).textureID,
		0
	);

	std::vector<GLenum> drawBuffers;
	for (auto& [attachment, texture] : m_attachments) {
		if (attachment != FrameBufferAttachment::DEPTH)
			drawBuffers.push_back(attachmentTypeToEnum(attachment));
	}
	glNamedFramebufferDrawBuffers(
		m_framebuffer, drawBuffers.size(), drawBuffers.data()
	);
}

void FrameBuffer::setResolution(
	glm::ivec2 resolution, TextureManager& textureManager
) {
	if (resolution == m_resolution) return;
	for (auto& [type, handle] : m_attachments) {
		Texture& texture = textureManager.getTexture(handle);
		if (resolution.x != texture.width && resolution.y != texture.height) {
			texture.width = resolution.x;
			texture.height = resolution.y;

			TextureManager::TextureSpecification specs {
				.definition = texture,
			};
			textureManager.createTexture(specs, handle);
		}

		setAttachment(type, handle, textureManager);
	}
	m_resolution = resolution;
}

void FrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glViewport(0, 0, m_resolution.x, m_resolution.y);

	if (m_clearOnBind) glClear(m_clearMask);
}
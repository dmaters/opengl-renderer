#pragma once

#include <glad/glad.h>

#include <filesystem>

struct Texture {
	GLuint textureID = 0;
	GLuint64 textureBindlessHandle = 0;

	GLint wrapping = GL_REPEAT;
	GLint filtering = GL_LINEAR;
	GLenum format = GL_RGBA16F;
	GLenum type = GL_TEXTURE_2D;
	GLint mipmap_levels = 1;

	GLsizei width = 0;
	GLsizei height = 0;
	GLsizei depth = 1;
};

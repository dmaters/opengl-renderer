#pragma once

#include <cstddef>
#include <vector>

#include "glad/glad.h"

class IndexBuffer {
private:
	GLuint m_glID;
	GLenum m_format;
	uint32_t m_count;

public:
	IndexBuffer(std::vector<std::byte>& data, GLenum format);
	inline GLuint getID() const { return m_glID; }
	inline uint32_t getCount() const { return m_count; }
	inline GLenum getFormat() const { return m_format; }
};

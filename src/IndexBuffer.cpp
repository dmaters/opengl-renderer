#include "IndexBuffer.h"

#include <glad/glad.h>

#include <cstddef>

constexpr uint32_t getCount(size_t size, GLenum format) {
	switch (format) {
		case GL_UNSIGNED_BYTE:
			return size / sizeof(unsigned char);
		case GL_UNSIGNED_SHORT:
			return size / sizeof(unsigned short);
		case GL_UNSIGNED_INT:
			return size / sizeof(uint32_t);
		default:
			return 0;
	}
}
IndexBuffer::IndexBuffer(std::vector<std::byte>& data, GLenum format) :
	m_format(format), m_count(::getCount(data.size(), format)) {
	glCreateBuffers(1, &m_glID);
	glNamedBufferStorage(m_glID, data.size(), data.data(), GL_MAP_READ_BIT);
}

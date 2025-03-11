#include "VertexBuffer.h"

#include <glad/glad.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

VertexBuffer::VertexBuffer(std::span<unsigned char> vertices) {
	glCreateBuffers(1, &m_glID);
	glNamedBufferStorage(
		m_glID, vertices.size(), vertices.data(), GL_MAP_READ_BIT
	);
}

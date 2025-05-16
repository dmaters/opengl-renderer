#include "VertexArray.h"

#include <glad/glad.h>

#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>

VertexArray::VertexArray(
	std::vector<std::byte>& vertices,
	std::vector<std::byte>& indices,
	GLenum indexFormat
) :
	m_vertexBuffer(vertices), m_indexBuffer(indices, indexFormat) {
	uint8_t size = sizeof(glm::vec3) * 3 + sizeof(glm::vec2);

	glCreateVertexArrays(1, &m_arrayID);
	glVertexArrayVertexBuffer(m_arrayID, 0, m_vertexBuffer.getID(), 0, size);
	glVertexArrayElementBuffer(m_arrayID, m_indexBuffer.getID());

	glEnableVertexArrayAttrib(m_arrayID, 0);
	glVertexArrayAttribFormat(m_arrayID, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(m_arrayID, 0, 0);

	glEnableVertexArrayAttrib(m_arrayID, 1);
	glVertexArrayAttribFormat(m_arrayID, 1, 3, GL_FLOAT, GL_FALSE, 12);
	glVertexArrayAttribBinding(m_arrayID, 1, 0);

	glEnableVertexArrayAttrib(m_arrayID, 2);
	glVertexArrayAttribFormat(m_arrayID, 2, 3, GL_FLOAT, GL_FALSE, 24);
	glVertexArrayAttribBinding(m_arrayID, 2, 0);

	glEnableVertexArrayAttrib(m_arrayID, 3);
	glVertexArrayAttribFormat(m_arrayID, 3, 2, GL_FLOAT, GL_FALSE, 36);
	glVertexArrayAttribBinding(m_arrayID, 3, 0);
}
void VertexArray::release() { glDeleteVertexArrays(1, &m_arrayID); }

void VertexArray::StaticMeshDefinition() {

};
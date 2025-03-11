#include "VertexArray.h"

#include <glad/glad.h>

#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>

VertexArray::VertexArray(Specifications& specs) :
	m_vertexBuffer(specs.vertices),
	m_indexBuffer(specs.indices, specs.indicesFormat) {
	switch (specs.type) {
		case Type::STATIC_MESH:
			StaticMeshDefinition();
			break;
	}
}
void VertexArray::release() { glDeleteVertexArrays(1, &m_arrayID); }

void VertexArray::StaticMeshDefinition() {
	glCreateVertexArrays(1, &m_arrayID);

	glVertexArrayVertexBuffer(
		m_arrayID, 0, m_vertexBuffer.getID(), 0, sizeof(StaticMeshLayout)
	);
	glVertexArrayElementBuffer(m_arrayID, m_indexBuffer.getID());

	glEnableVertexArrayAttrib(m_arrayID, 0);
	glVertexArrayAttribFormat(
		m_arrayID,
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		offsetof(StaticMeshLayout, position)
	);
	glVertexArrayAttribBinding(m_arrayID, 0, 0);

	glEnableVertexArrayAttrib(m_arrayID, 1);
	glVertexArrayAttribFormat(
		m_arrayID, 1, 3, GL_FLOAT, GL_FALSE, offsetof(StaticMeshLayout, normal)
	);
	glVertexArrayAttribBinding(m_arrayID, 1, 0);

	glEnableVertexArrayAttrib(m_arrayID, 2);
	glVertexArrayAttribFormat(
		m_arrayID,
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		offsetof(StaticMeshLayout, texcoord)
	);
	glVertexArrayAttribBinding(m_arrayID, 2, 0);
};
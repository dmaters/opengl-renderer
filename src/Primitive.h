#pragma once

#include <cstdint>

#include "Node.h"
#include "Resources.h"
#include "VertexArray.h"

class Primitive : public Node {
public:
	struct ViewProjectionUniformBuffer {
		glm::mat4 u_view;
		glm::mat4 u_projection;
	};

private:
	VertexArray m_vertexArray;
	uint32_t m_materialIndex;
	glm::vec4 m_collider = glm::vec4(0);

public:
	Primitive(
		VertexArray vertexArray, uint32_t materialIndex, glm::vec4 collider
	) :
		m_vertexArray(vertexArray),
		m_materialIndex(materialIndex),
		m_collider(collider) {}

	void setCollider(glm::vec4 collider) { m_collider = collider; }

	uint32_t getMaterialIndex() const { return m_materialIndex; }
	const VertexArray& getVertexArray() const { return m_vertexArray; }
	glm::vec4 getCollider() const { return m_collider; }
};
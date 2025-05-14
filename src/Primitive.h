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
	float m_size = 0;

public:
	Primitive(VertexArray vertexArray, uint32_t materialIndex, float size) :
		m_vertexArray(vertexArray),
		m_materialIndex(materialIndex),
		m_size(size) {}

	void setSize(float size) { m_size = size; }

	uint32_t getMaterialIndex() const { return m_materialIndex; }
	const VertexArray& getVertexArray() const { return m_vertexArray; }
	float getSize() const { return m_size; }
};
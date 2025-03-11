#pragma once

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
	glm::mat4 m_modelMatrix;
	VertexArray m_vertexArray;
	MaterialHandle m_material;
	float m_size = 0;

public:
	Primitive(
		VertexArray vertexArray, MaterialHandle materialHandle, float size
	) :
		m_vertexArray(vertexArray), m_material(materialHandle), m_size(size) {}

	inline void setModelMatrix(glm::mat4 modelMatrix) {
		m_modelMatrix = modelMatrix;
	}
	inline void setSize(float size) { m_size = size; }

	inline const glm::mat4& getModelMatrix() const { return m_modelMatrix; }
	inline MaterialHandle getMaterialIndex() const { return m_material; }
	inline const VertexArray& getVertexArray() const { return m_vertexArray; }
	inline const float getSize() const { return m_size; }
};
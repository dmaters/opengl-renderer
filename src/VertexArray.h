#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <span>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

class VertexArray {
public:
	enum Type {
		STATIC_MESH
	};

	struct StaticMeshLayout {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};
	struct Specifications {
		Type type;
		std::span<unsigned char> vertices;
		std::span<unsigned char> indices;
		GLenum indicesFormat;
	};

private:
	GLuint m_arrayID;
	uint32_t m_meshID = 0;

	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	void StaticMeshDefinition();

public:
	VertexArray(Specifications& specs);
	inline uint32_t getIndexCount() const { return m_indexBuffer.getCount(); }
	inline GLenum getIndexFormat() const { return m_indexBuffer.getFormat(); }
	inline GLuint getID() const { return m_arrayID; }

	void bind() const { glBindVertexArray(m_arrayID); }

	void release();
};

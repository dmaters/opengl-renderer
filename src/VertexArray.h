#pragma once
#include <cstddef>
#include <glm/glm.hpp>
#include <memory>
#include <span>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "glad/glad.h"

class VertexArray {
private:
	GLuint m_arrayID;
	uint32_t m_meshID = 0;

	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	void StaticMeshDefinition();

public:
	VertexArray(
		std::vector<std::byte>& vertices,
		std::vector<std::byte>& indices,
		GLenum indexFormat
	);
	inline uint32_t getIndexCount() const { return m_indexBuffer.getCount(); }
	inline GLenum getIndexFormat() const { return m_indexBuffer.getFormat(); }
	inline GLuint getID() const { return m_arrayID; }

	void bind() const { glBindVertexArray(m_arrayID); }

	void release();
};

#pragma once

#include <cstddef>
#include <vector>

#include "glad/glad.h"

class VertexBuffer {
private:
	GLuint m_glID;

public:
	VertexBuffer(std::vector<std::byte>& data);

	inline GLuint getID() const { return m_glID; }
};

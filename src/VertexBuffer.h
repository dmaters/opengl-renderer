#pragma once

#include <map>
#include <span>

#include "glad/glad.h"

class VertexBuffer {
private:
	GLuint m_glID;

public:
	VertexBuffer(std::span<unsigned char> data);

	inline GLuint getID() const { return m_glID; }
};

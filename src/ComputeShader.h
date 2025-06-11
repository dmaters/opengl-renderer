#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>

#include "Program.h"

class ComputeShader : public Program {
public:
	ComputeShader(std::filesystem::path path) : Program(path) {}
	void dispatch(glm::ivec3 axis);

	using Program::setUBO;
	using Program::setUniform;

	void setUniform(
		std::string name, Texture& texture, GLenum usage, uint32_t mipmap = 0
	);
};
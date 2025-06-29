#pragma once
#include <filesystem>
#include <glm/glm.hpp>
#include <string>

#include "Texture.h"
#include "glad/glad.h"
#include "glm/fwd.hpp"

class Program {
public:
	class DefaultPrograms;
	struct Stages {
		std::filesystem::path vertex = "";
		std::filesystem::path geometry = "";
		std::filesystem::path fragment = "";

		bool operator<(const Stages& other) const {
			// Lexicographic comparison of the three strings
			if (vertex != other.vertex) return vertex < other.vertex;
			if (geometry != other.geometry) return geometry < other.geometry;
			return fragment < other.fragment;
		}
	};

protected:
	GLuint m_programID;

private:
	void loadShader(std::filesystem::path path, GLenum shaderType);

public:
	Program(Stages stages);
	Program(std::filesystem::path compute);

	void bind() const;

	void setUniform(std::string name, glm::mat4 value);
	void setUniform(std::string name, glm::mat3 value);

	void setUniform(std::string name, glm::vec4 value);
	void setUniform(std::string name, glm::vec3 value);
	void setUniform(std::string name, glm::vec2 value);
	void setUniform(std::string name, float value);
	void setUniform(std::string name, int value);
	void setUniform(std::string name, unsigned int value);
	void setUniform(std::string name, Texture& value);
	void setUniform_const(std::string name, const Texture& value);

	void setUBO(std::string name, GLuint UBO, GLuint binding);
};

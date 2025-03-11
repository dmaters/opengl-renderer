#include "Program.h"

#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

Program::Program(Program::Stages stages) {
	m_programID = glCreateProgram();

	if (!stages.vertex.empty()) loadShader(stages.vertex, GL_VERTEX_SHADER);

	if (!stages.geometry.empty())
		loadShader(stages.geometry, GL_GEOMETRY_SHADER);

	if (!stages.fragment.empty())
		loadShader(stages.fragment, GL_FRAGMENT_SHADER);

	glLinkProgram(m_programID);
	GLint isLinked = 0;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &isLinked);
	if (!isLinked) {
		GLint maxLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(m_programID, maxLength, &maxLength, &infoLog[0]);

		std::cout << "Error linking for program:" << std::endl;
		std::cout << "Vertex: " << stages.vertex << std::endl;
		std::cout << "Fragment: " << stages.fragment << std::endl;
		std::cout << infoLog.data() << std::endl;

		throw "Program did not compile";
	}
}
void Program::loadShader(std::filesystem::path path, GLenum shaderType) {
	std::string code;

	std::ifstream file;

	try {
		file.open(path);
		std::stringstream shaderStream;
		shaderStream << file.rdbuf();
		file.close();
		code = shaderStream.str();
	}

	catch (std::ifstream::failure e) {
		std::cout << "Error Shaders not loaded correctly" << std::endl;
		throw;
	}

	const char* vShaderCode = code.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &vShaderCode, 0);
	glCompileShader(shader);

	int success;
	char log[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, log);
		std::cout << log << std::endl;
	}
	glAttachShader(m_programID, shader);
	// glDeleteShader(shader);
}

Program::Program(std::filesystem::path compute) {
	m_programID = glCreateProgram();

	loadShader(compute, GL_COMPUTE_SHADER);
	glLinkProgram(m_programID);
	GLint isLinked = 0;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &isLinked);
	if (!isLinked) {
		GLint maxLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(m_programID, maxLength, &maxLength, &infoLog[0]);

		throw "Compute shader link failed";
	}
	std::vector<GLuint> shaders(6);
}

// Program::~Program() { glDeleteProgram(m_programID); }

void Program::bind() const {
	GLint currentProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

	if (currentProgram == m_programID) return;

	glUseProgram(m_programID);
}

void Program::setUBO(std::string name, GLuint id, GLuint binding) {
	uint32_t blockIndex = glGetUniformBlockIndex(m_programID, name.c_str());
	glUniformBlockBinding(m_programID, blockIndex, binding);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
}

void Program::setUniform(std::string name, glm::mat4 value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniformMatrix4fv(m_programID, location, 1, GL_FALSE, &value[0][0]);
}
void Program::setUniform(std::string name, glm::mat3 value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniformMatrix3fv(m_programID, location, 1, GL_FALSE, &value[0][0]);
}
void Program::setUniform(std::string name, glm::vec4 value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniform4fv(m_programID, location, 1, &value[0]);
}

void Program::setUniform(std::string name, glm::vec3 value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniform3fv(m_programID, location, 1, &value[0]);
}

void Program::setUniform(std::string name, glm::vec2 value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniform2fv(m_programID, location, 1, &value[0]);
}

void Program::setUniform(std::string name, float value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniform1f(m_programID, location, value);
}

void Program::setUniform(std::string name, int value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniform1i(m_programID, location, value);
}

void Program::setUniform(std::string name, Texture& value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniformHandleui64ARB(
		m_programID, location, value.textureBindlessHandle
	);
}

void Program::setUniform_const(std::string name, const Texture& value) {
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniformHandleui64ARB(
		m_programID, location, value.textureBindlessHandle
	);
}

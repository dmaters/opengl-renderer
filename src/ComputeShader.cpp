#include "ComputeShader.h"

#include <glad/glad.h>

#include <string>

#include "Program.h"

void ComputeShader::dispatch(glm::ivec3 groupCount) {
	bind();
	glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ComputeShader::setUniform(
	std::string name, Texture& texture, GLenum usage, uint32_t mipmap
) {
	GLuint64 imageHandle = glGetImageHandleARB(
		texture.textureID,
		0,
		texture.type == GL_TEXTURE_CUBE_MAP,
		0,
		texture.format
	);
	glMakeImageHandleResidentARB(imageHandle, usage);
	uint32_t location = glGetUniformLocation(m_programID, name.c_str());
	glProgramUniformHandleui64ARB(m_programID, location, imageHandle);
}
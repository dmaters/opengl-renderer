#include "SinglePassComputeEffect.h"

void SinglePassComputeEffect::run(Texture& input, Texture& output) {
	m_program->bind();

	m_program->setUniform("u_input", input, GL_READ_ONLY, 0);
	m_program->setUniform("u_output", output, GL_WRITE_ONLY, 0);

	m_program->dispatch({ output.width, output.height, 1 });
}
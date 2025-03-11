#include "Blur.h"

#include "SinglePassComputeEffect.h"

/**
 * \brief Creates a new Blur effect with the given blur size
 *
 * The blur size is the number of pixels on each side of the blur
 * kernel.
 */
Blur::Blur(int blurSize) :
	SinglePassComputeEffect(
		std::filesystem::path("resources/shaders/compute/blur.glsl")
	),
	m_blurSize(blurSize) {}

void Blur::run(Texture& input, Texture& output) {
	m_program->setUniform("u_blurSize", m_blurSize);
	
	SinglePassComputeEffect::run(input, output);
}
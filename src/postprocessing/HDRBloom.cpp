#include "HDRBloom.h"

#include <memory>

#include "ResourceManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include "glad/glad.h"

HDRBloom::HDRBloom(std::shared_ptr<ResourceManager> resourceManager) :
	PostProcessingEffect(),
	m_brightPass("resources/shaders/bloom/brightPass.glsl"),
	m_blur(std::make_unique<ComputeShader>("resources/shaders/compute/blur.glsl"
    )),
	m_downSample(std::make_unique<ComputeShader>(
		"resources/shaders/compute/downsample.glsl"
	)),
	m_upSample(std::make_unique<ComputeShader>(
		"resources/shaders/bloom/upSamplePass.glsl"
	)),
	m_add(std::make_unique<ComputeShader>("resources/shaders/compute/add.glsl")
    ),
	m_resourceManager(resourceManager)

{
	m_resourceManager = resourceManager;

	TextureManager& textureManager = m_resourceManager->getTextureManager();

	TextureManager::TextureSpecification textureSpecs {
		.definition = {
			.format = GL_RGBA32F,
			.mipmap_levels = 6,
			.width = 800,
			.height = 600,
		}
	};
	m_workTexture = textureManager.createTexture(textureSpecs);
	m_temporary = textureManager.createTexture(textureSpecs);
}

void HDRBloom::run(Texture& input, Texture& output) {
	TextureManager& textureManager = m_resourceManager->getTextureManager();
	Texture& temporary = textureManager.getTexture(m_temporary);
	Texture& workTexture = textureManager.getTexture(m_workTexture);

	m_brightPass.run(input, temporary);

	m_blur->setUniform("u_input", temporary, GL_READ_ONLY);
	m_blur->setUniform("u_output", workTexture, 0, GL_WRITE_ONLY);
	m_blur->setUniform("u_blurSize", 4);
	m_blur->dispatch({ workTexture.width, workTexture.height, 1 });

	for (size_t i = 1; i <= 6; i++) {
		m_downSample->setUniform("u_input", workTexture, GL_READ_ONLY, i - 1);
		m_downSample->setUniform("u_output", temporary, GL_WRITE_ONLY, i);
		m_downSample->dispatch(
			{ workTexture.width / pow(2, i), workTexture.height / pow(2, i), 1 }
		);

		m_blur->setUniform("u_input", temporary, GL_READ_ONLY, i);
		m_blur->setUniform("u_output", workTexture, GL_WRITE_ONLY, i);
		m_blur->setUniform("u_blurSize", 4);

		m_blur->dispatch(
			{ workTexture.width / pow(2, i), workTexture.height / pow(2, i), 1 }
		);
	}

	for (int i = 5; i >= 0; i--) {
		m_upSample->setUniform("u_input", workTexture, GL_READ_ONLY, i + 1);
		m_upSample->setUniform("u_output", temporary, GL_WRITE_ONLY, i);
		if (i == -1)
			m_upSample->setUniform("u_intermediate", input, GL_READ_ONLY);
		else
			m_upSample->setUniform(
				"u_intermediate", workTexture, GL_READ_ONLY, i
			);

		m_upSample->dispatch({ workTexture.width / pow(2, i + 1),
		                       workTexture.height / pow(2, i + 1),
		                       1 });

		glCopyImageSubData(
			temporary.textureID,
			GL_TEXTURE_2D,
			i,
			0,
			0,
			0,
			workTexture.textureID,
			GL_TEXTURE_2D,
			i,
			0,
			0,
			0,
			workTexture.width / pow(2, i),
			workTexture.height / pow(2, i),
			1
		);
	}

	m_add->setUniform("u_input", workTexture, 0, GL_READ_ONLY);
	m_add->setUniform("u_output", input, 0, GL_READ_ONLY);
	m_add->setUniform("u_intermediate", output, 0, GL_WRITE_ONLY);
	m_add->dispatch({ input.width, input.height, 1 });
}

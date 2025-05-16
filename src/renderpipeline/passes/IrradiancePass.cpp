#include "IrradiancePass.h"

#include "ResourceManager.h"
#include "Resources.h"
#include "TextureManager.h"
#include "glad/glad.h"
#include "glm/fwd.hpp"

IrradiancePass::IrradiancePass(
	TextureHandle skybox, ResourceManager& resourceManager
) :
	m_diffusePass("resources/shaders/irradiance.comp"),
	m_specularPass("resources/shaders/irradiance_specular.comp") {
	TextureManager& textureManager = resourceManager.getTextureManager();
	m_diffuseMap = textureManager.createTexture({
		.definition = {
					   .wrapping = GL_CLAMP_TO_EDGE,
					   .format = GL_RGBA16F,
					   .type = GL_TEXTURE_CUBE_MAP,
					   .width = 32,
					   .height = 32,
					   }
    });
	m_specularMap = textureManager.createTexture({
		.definition = {
					   .wrapping = GL_CLAMP_TO_EDGE,
					   .format = GL_RGBA16F,
					   .type = GL_TEXTURE_CUBE_MAP,
					   .mipmap_levels = 5,
					   .width = 512,
					   .height = 512,
					   }
    });

	m_diffusePass.setUniform(
		"irradiance_map",
		textureManager.getTexture(m_diffuseMap),
		GL_WRITE_ONLY,
		0
	);
	m_diffusePass.setUniform(
		"environment_map", textureManager.getTexture(skybox)
	);
	m_specularPass.setUniform(
		"environment_map", textureManager.getTexture(skybox)
	);

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Texture& diffuseMap = textureManager.getTexture(m_diffuseMap);
	glClearTexImage(diffuseMap.textureID, 0, GL_RGBA, GL_FLOAT, clearColor);
	Texture& specularMap = textureManager.getTexture(m_specularMap);
	for (int i = 0; i < 5; i++) {
		glClearTexImage(
			specularMap.textureID, i, GL_RGBA, GL_FLOAT, clearColor
		);
	}
}

void IrradiancePass::computeIrradiance(ResourceManager& resourceManager) {
	m_diffusePass.dispatch(glm::ivec3(32, 32, 6));

	for (int i = 0; i < 5; i++) {
		m_specularPass.setUniform("current_level", i);
		m_specularPass.setUniform(
			"irradiance_map",
			resourceManager.getTextureManager().getTexture(m_specularMap),
			GL_WRITE_ONLY,
			i
		);
		m_specularPass.dispatch(glm::ivec3(512 / pow(2, i), 512 / pow(2, i), 6)
		);
	}
}
#include "IrradiancePass.h"

#include "ResourceManager.h"
#include "Resources.h"
#include "TextureManager.h"
#include "glm/fwd.hpp"

IrradiancePass::IrradiancePass(
	TextureHandle skybox, ResourceManager& resourceManager
) :
	m_pass("resources/shaders/irradiance.comp") {
	TextureManager& textureManager = resourceManager.getTextureManager();
	m_map = textureManager.createTexture({
		.definition = {
					   .format = GL_RGBA16F,
					   .type = GL_TEXTURE_CUBE_MAP,
					   .width = 32,
					   .height = 32,
					   }
    });

	m_pass.setUniform(
		"irradiance_map", textureManager.getTexture(m_map), GL_WRITE_ONLY, 0
	);
	m_pass.setUniform("environment_map", textureManager.getTexture(skybox));

	Texture& irradianceMap = textureManager.getTexture(m_map);
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearTexImage(irradianceMap.textureID, 0, GL_RGBA, GL_FLOAT, clearColor);
}

TextureHandle IrradiancePass::computeIrradiance() {
	m_pass.dispatch(glm::ivec3(32, 32, 6));
	return m_map;
}
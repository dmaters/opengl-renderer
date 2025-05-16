#pragma once

#include "ComputeShader.h"
#include "ResourceManager.h"
#include "Resources.h"

class IrradiancePass {
public:
private:
	TextureHandle m_diffuseMap;
	TextureHandle m_specularMap;
	ComputeShader m_diffusePass;
	ComputeShader m_specularPass;

public:
	IrradiancePass(TextureHandle skybox, ResourceManager& resourceManager);
	void computeIrradiance(ResourceManager& resourceManager);

	TextureHandle getDiffuseMap() const { return m_diffuseMap; }
	TextureHandle getSpecularMap() const { return m_specularMap; }
};

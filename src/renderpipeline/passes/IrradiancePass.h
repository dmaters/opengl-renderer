#pragma once

#include "ComputeShader.h"
#include "ResourceManager.h"
#include "Resources.h"

class IrradiancePass {
private:
	TextureHandle m_map;
	ComputeShader m_pass;

public:
	IrradiancePass(TextureHandle skybox, ResourceManager& resourceManager);

	TextureHandle computeIrradiance();
};

#pragma once

#include "Material.h"
#include "SinglePassPostProcessingEffect.h"

class HDRToneMapping : public SinglePassPostProcessingEffect<Material> {
private:
public:
	HDRToneMapping() {}
	HDRToneMapping(std::string shaderPath);

	void run(Texture& input, Texture& output) override;
};
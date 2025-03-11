#pragma once

#include <memory>

#include "ComputeShader.h"
#include "PostProcessingEffect.h"
#include "ResourceManager.h"
#include "SinglePassComputeEffect.h"
#include "Texture.h"

class HDRBloom : public PostProcessingEffect {
private:
	SinglePassComputeEffect m_brightPass;

	std::unique_ptr<ComputeShader> m_blur;
	std::unique_ptr<ComputeShader> m_downSample;
	std::unique_ptr<ComputeShader> m_upSample;
	std::unique_ptr<ComputeShader> m_add;

	TextureHandle m_workTexture;
	TextureHandle m_temporary;
	std::shared_ptr<ResourceManager> m_resourceManager;

	float m_bloomThreshold;

public:
	HDRBloom(std::shared_ptr<ResourceManager> resourceManager);

	void run(Texture& input, Texture& output) override;
	inline ~HDRBloom() {}
};
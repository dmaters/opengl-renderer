#pragma once

#include <filesystem>
#include <memory>

#include "ComputeShader.h"
#include "PostProcessingEffect.h"
#include "Texture.h"

class SinglePassComputeEffect : public PostProcessingEffect {
protected:
	std::unique_ptr<ComputeShader> m_program;

public:
	inline SinglePassComputeEffect(std::filesystem::path shader_path) :
		m_program(std::make_unique<ComputeShader>(shader_path)) {}

	void run(Texture& input, Texture& output) override;
};

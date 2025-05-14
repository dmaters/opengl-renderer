#pragma once

#include <array>

#include "ResourceManager.h"
#include "Resources.h"
#include "renderpipeline/BasePass.h"
#include "renderpipeline/FrameBuffer.h"

class ShadowPass {
private:
	FrameBuffer m_shadowMapFB;
	MaterialHandle m_shadowMaterial;
	MaterialHandle m_omniShadowMaterial;

	std::optional<BasePass> m_shadowPass;
	std::optional<BasePass> m_omniShadowPass;

	struct CubemapTrasforms;

public:
	ShadowPass(ResourceManager& resourceManager);

	void render(Scene& scene, ResourceManager& resourceManager);
};

struct ShadowPass::CubemapTrasforms {
	std::array<glm::mat4, 6> rotations;

	static CubemapTrasforms get() {
		CubemapTrasforms transforms;

		transforms.rotations[0] = glm::lookAt(
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(1.0, 0.0, 0.0),
			glm::vec3(0.0, 1.0, 0.0)
		);

		transforms.rotations[1] = glm::lookAt(
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(-1.0, 0.0, 0.0),
			glm::vec3(0.0, 1.0, 0.0)
		);
		transforms.rotations[2] = glm::lookAt(
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.0, -1.0, 0.0),
			glm::vec3(0.0, 0.0, 1.0)
		);
		transforms.rotations[3] = glm::lookAt(
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0, 1.0, 0.0),
			glm::vec3(0.0, 0.0, 1.0)
		);
		transforms.rotations[4] = glm::lookAt(
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.0, 0.0, 1.0),
			glm::vec3(0.0, 1.0, 0.0)
		);
		transforms.rotations[5] = glm::lookAt(
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.0, 0.0, -1.0),
			glm::vec3(0.0, 1.0, 0.0)
		);

		return transforms;
	}
};
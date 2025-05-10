#pragma once

#include <glad/glad.h>

#include <array>
#include <glm/glm.hpp>
#include <memory>

#include "ComputeShader.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"

class RenderPipeline {
public:
	struct RenderSpecifications;

private:
	struct CubemapTrasforms;

	FrameBuffer m_shadowMapFB;
	FrameBuffer m_gbufferFB;
	FrameBuffer m_finalFB;

	MaterialHandle m_shadowMapMaterial;
	MaterialHandle m_omniShadowMapMaterial;

	MaterialHandle m_compositionMaterial;
	MaterialHandle m_skyboxMaterial;

	MaterialHandle m_lightingMaterial;

	UBOHandle m_sceneLightUBO;
	UBOHandle m_lightsUBO;

	std::unique_ptr<ComputeShader> m_irradianceCompute;

	GLuint m_quadVao = 0;

	bool m_shadowmapsGenerated = false;
	bool m_irradianceGenerated = false;

	void renderShadowMap(Scene& scene);

	void setupShadowMapPass();
	void setupForwardPass();

	std::shared_ptr<ResourceManager> m_resourceManager;
	void renderShadowMaps(RenderSpecifications& scene);

public:
	RenderPipeline(std::shared_ptr<ResourceManager> resourceManager);

	void render(RenderSpecifications& specs);
	void renderSubpass(RenderPassSpecs& renderPassSpecs);
	void renderFullscreenPass(MaterialHandle handle, bool testDepth = false);
};

struct RenderPipeline::RenderSpecifications {
	Scene& scene;
	glm::ivec2 resolution;
};

struct RenderPipeline::CubemapTrasforms {
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
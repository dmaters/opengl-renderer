#pragma once

#include <memory>
#include <optional>

#include "Material.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "postprocessing/HDRBloom.h"
#include "renderpipeline/RenderPipeline.h"

class Renderer {
private:
	std::shared_ptr<ResourceManager> m_resourceManager;

	std::unique_ptr<RenderPipeline> m_renderPipeline;

	std::unique_ptr<HDRBloom> m_bloom;

	glm::mat4 m_projection, m_view;

	int m_width, m_height;

	GLuint m_quadVAO = 0;
	Program m_quadMaterial;

public:
	Renderer(
		int width, int height, std::shared_ptr<ResourceManager> resourceManager
	);

	void setResolution(int width, int height);
	void render(Scene& scene);

	void drawQuad(Texture& texture);
};

#pragma once

#include <glad/glad.h>

#include <chrono>
#include <memory>

#include "Material.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "renderpipeline/RenderPipeline.h"

class Renderer {
private:
	std::shared_ptr<ResourceManager> m_resourceManager;
	std::unique_ptr<RenderPipeline> m_renderPipeline;

	glm::mat4 m_projection, m_view;

	int m_width, m_height;

	std::array<GLuint, 3> m_queryIDs;
	std::chrono::time_point<std::chrono::steady_clock> m_lastCPUTick;

public:
	Renderer(
		int width, int height, std::shared_ptr<ResourceManager> resourceManager
	);

	void setResolution(int width, int height);
	void render(Scene& scene);
};

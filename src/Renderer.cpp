
#include "Renderer.h"

#include <memory>

#include "Camera.h"
#include "Primitive.h"
#include "Program.h"
#include "RenderPipeline/RenderPipeline.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "glad/glad.h"
#include "postprocessing/HDRBloom.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

Renderer::Renderer(
	int width, int height, std::shared_ptr<ResourceManager> resourceManager
) :
	m_resourceManager(resourceManager),
	m_renderPipeline(std::make_unique<RenderPipeline>(m_resourceManager)),
	m_bloom(std::make_unique<HDRBloom>(m_resourceManager)) {
	setResolution(width, height);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::render(Scene& scene) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Camera& camera = scene.getCamera();

	m_view = camera.getTransformationMatrix(true);

	Primitive::ViewProjectionUniformBuffer projectionViewUBOHandle = {
		m_view, m_projection
	};

	m_resourceManager->updateUBO(
		UBOHandle::PROJECTION_VIEW, projectionViewUBOHandle
	);

	RenderPipeline::RenderSpecifications specs = {
		.scene = scene, .resolution = glm::ivec2(m_width, m_height)
	};

	m_renderPipeline->render(specs);
}

void Renderer::setResolution(int width, int height) {
	m_width = width;
	m_height = height;
	m_projection = glm::perspective(
		glm::radians(60.0f), (float)width / (float)height, 0.1f, 10000.0f
	);
}
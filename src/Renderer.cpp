
#include "Renderer.h"

#include <memory>

#include "Camera.h"
#include "Primitive.h"
#include "Program.h"
#include "RenderPipeline/RenderPipeline.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "TextureManager.h"
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
	m_bloom(std::make_unique<HDRBloom>(m_resourceManager)),
	m_quadMaterial(Program::Stages {
		.vertex = Program::DefaultPrograms::QUAD::VERTEX,
		.fragment = Program::DefaultPrograms::QUAD::FRAGMENT }) {
	setResolution(width, height);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::render(Scene& scene) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	TextureManager& textureManager = m_resourceManager->getTextureManager();

	Camera& camera = scene.getCamera();

	m_view = camera.getTransformationMatrix();

	Primitive::ViewProjectionUniformBuffer projectionViewUBOHandle = {
		m_view, m_projection
	};

	m_resourceManager->updateUBO(
		UBOHandle::PROJECTION_VIEW, projectionViewUBOHandle
	);

	RenderPipeline::RenderSpecifications specs = {
		.scene = scene, .resolution = glm::ivec2(m_width, m_height)
	};

	TextureHandle output = m_renderPipeline->render(specs);

	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//  drawQuad(textureManager.getTexture(output));
}

void Renderer::setResolution(int width, int height) {
	m_width = width;
	m_height = height;
	m_projection = glm::perspective(
		glm::radians(60.0f), (float)width / (float)height, 0.1f, 10000.0f
	);
}

void Renderer::drawQuad(Texture& texture) {
	if (m_quadVAO == 0) {
		glGenVertexArrays(1, &m_quadVAO);
	}

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(m_quadVAO);
	m_quadMaterial.setUniform("u_Texture", texture);
	m_quadMaterial.bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEnable(GL_DEPTH_TEST);
}

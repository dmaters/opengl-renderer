
#include "Renderer.h"

#include <cstdint>
#include <memory>

#include "Camera.h"
#include "Primitive.h"
#include "Program.h"
#include "RenderPipeline/RenderPipeline.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "State.h"
#include "UI/UI.h"
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
	m_renderPipeline(std::make_unique<RenderPipeline>(m_resourceManager)) {
	setResolution(width, height);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenQueries(m_queryIDs.size(), m_queryIDs.data());

	m_lastCPUTick = std::chrono::high_resolution_clock::now();
	glBeginQuery(GL_TIME_ELAPSED, m_queryIDs[0]);
}

void Renderer::render(Scene& scene) {
	State& state = State::Get();

	glBeginQuery(
		GL_TIME_ELAPSED, m_queryIDs[state.frameIndex % m_queryIDs.size()]
	);

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

	uint8_t timeWindowIndex = state.frameIndex % 128;
	{  // CPU
		auto cpuEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> cpuElapsed =
			cpuEnd - m_lastCPUTick;
		m_lastCPUTick = std::chrono::high_resolution_clock::now();

		state.averageTimeCPU -= state.frameTimesCPU[timeWindowIndex] / 128;
		state.averageTimeCPU += cpuElapsed.count() / 128;
		state.frameTimesCPU[timeWindowIndex] = cpuElapsed.count();
	}

	{  // GPU

		glEndQuery(GL_TIME_ELAPSED);

		uint8_t oldestFrameIndex = (state.frameIndex + 1) % m_queryIDs.size();
		GLint available = 0;
		if (state.frameIndex > m_queryIDs.size())
			glGetQueryObjectiv(
				m_queryIDs[oldestFrameIndex],
				GL_QUERY_RESULT_AVAILABLE,
				&available
			);

		uint64_t timeElapsed = 0;
		if (available) {
			glGetQueryObjectui64v(
				m_queryIDs[oldestFrameIndex], GL_QUERY_RESULT, &timeElapsed
			);
		}

		float gpuTime = timeElapsed / 1e6;
		state.averageTimeGPU -= state.frameTimesGPU[timeWindowIndex] / 128;
		state.averageTimeGPU += gpuTime / 128;
		state.frameTimesGPU[timeWindowIndex] = gpuTime;
	}

	state.frameIndex++;
}

void Renderer::setResolution(int width, int height) {
	m_width = width;
	m_height = height;
	m_projection = glm::perspective(
		glm::radians(60.0f), (float)width / (float)height, 0.1f, 10000.0f
	);
}
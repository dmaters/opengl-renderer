
#include "RenderPipeline.h"

#include <glad/glad.h>

#include <functional>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "ComputeShader.h"
#include "FrameBuffer.h"
#include "Frustum.hpp"
#include "Light.h"
#include "Material.h"
#include "Node.h"
#include "Primitive.h"
#include "Program.h"
#include "RenderPass.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "TextureManager.h"

RenderPipeline::RenderPipeline(std::shared_ptr<ResourceManager> resourceManager
) :
	m_resourceManager(resourceManager) {
	TextureManager& textureManager = resourceManager->getTextureManager();
	m_shadowMapFB =
		FrameBuffer::getShadowMapFB(textureManager, glm::uvec2(512, 512));

	m_gbufferFB =
		FrameBuffer::getGBufferPassFB(textureManager, glm::uvec2(1, 1));
	m_finalFB =
		FrameBuffer::getGeneralRenderPassFB(textureManager, glm::uvec2(1, 1));

	m_finalFB.setAttachment(
		FrameBufferAttachment::DEPTH,
		m_gbufferFB.getAttachment(FrameBufferAttachment::DEPTH),
		m_resourceManager->getTextureManager()
	);

	TextureHandle irradianceMapHandle = textureManager.createTexture({
		.definition = {
					   .format = GL_RGBA16F,
					   .type = GL_TEXTURE_CUBE_MAP,
					   .width = 32,
					   .height = 32,
					   }
    }, TextureHandle::IRRADIANCE);

	m_irradianceCompute =
		std::make_unique<ComputeShader>("resources/shaders/irradiance.comp");
	m_irradianceCompute->setUniform(
		"irradiance_map",
		textureManager.getTexture(TextureHandle::IRRADIANCE),
		GL_WRITE_ONLY,
		0

	);
	TextureHandle skyboxHandle =
		textureManager.loadTexture("resources/textures/skybox.hdr");

	m_irradianceCompute->setUniform(
		"environment_map",
		m_resourceManager->getTextureManager().getTexture(skyboxHandle)
	);

	Texture& irradianceMap =
		m_resourceManager->getTextureManager().getTexture(irradianceMapHandle);
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // RGBA clear color
	glClearTexImage(irradianceMap.textureID, 0, GL_RGBA, GL_FLOAT, clearColor);

	ProgramHandle shadowProgram = resourceManager->registerProgram(
		{ .vertex = "resources/shaders/shadow.vert",
	      .fragment = "resources/shaders/shadow.frag" }
	);

	m_shadowMapMaterial =
		m_resourceManager->registerMaterial(Material(shadowProgram));

	ProgramHandle omniShadowProgram = resourceManager->registerProgram(
		{ .vertex = "resources/shaders/shadow_omni.vert",
	      .geometry = "resources/shaders/shadow_omni.geom",
	      .fragment = "resources/shaders/shadow.frag"

	    }
	);
	m_omniShadowMapMaterial =
		m_resourceManager->registerMaterial(Material(omniShadowProgram));

	auto cubemap_tranform_ubo =
		resourceManager->registerUBO(CubemapTrasforms::get(), 2);
	m_resourceManager->getMaterial(m_omniShadowMapMaterial)
		.setUniform("shadow_matrices", cubemap_tranform_ubo);

	m_lightsUBO =
		m_resourceManager->registerUBO(Light::LightUniformBuffer {}, 3);
	Material& standardMaterial =
		m_resourceManager->getMaterial(MaterialHandle::DEFAULT);
	standardMaterial.setUniform("LightsData", m_lightsUBO);
	m_resourceManager->getMaterial(m_omniShadowMapMaterial)
		.setUniform("LightsData", m_lightsUBO);
	m_resourceManager->getMaterial(m_shadowMapMaterial)
		.setUniform("LightsData", m_lightsUBO);

	ProgramHandle skyboxProgram = m_resourceManager->registerProgram(
		{ .vertex = "resources/shaders/skybox.vert",
	      .fragment = "resources/shaders/skybox.frag" }
	);

	m_skyboxMaterial =
		m_resourceManager->registerMaterial(Material(skyboxProgram));

	Material& skyboxMaterial = resourceManager->getMaterial(m_skyboxMaterial);
	skyboxMaterial.setUniform("skybox", skyboxHandle);
	skyboxMaterial.setUniform("projection_view", UBOHandle::PROJECTION_VIEW);

	ProgramHandle compositionProgram = m_resourceManager->registerProgram(
		{ .vertex = "resources/shaders/quad.vert",
	      .fragment = "resources/shaders/comp.frag" }
	);
	m_compositionMaterial =
		m_resourceManager->registerMaterial(Material(compositionProgram));
	Material& compMaterial =
		m_resourceManager->getMaterial(m_compositionMaterial);
	compMaterial.setUniform(
		"attachment", m_finalFB.getAttachment(FrameBufferAttachment::COLOR0)
	);

	ProgramHandle lightingProgram = m_resourceManager->registerProgram({
		.vertex = "resources/shaders/quad.vert",
		.fragment = "resources/shaders/lighting_pbr.frag",
	});
	m_lightingMaterial =
		m_resourceManager->registerMaterial(Material(lightingProgram));
	Material& lightingMaterial =
		m_resourceManager->getMaterial(m_lightingMaterial);
	lightingMaterial.setUniform(
		"_albedo", m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR0)
	);
	lightingMaterial.setUniform(
		"_normal", m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR1)
	);
	lightingMaterial.setUniform(
		"_world_position",
		m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR2)
	);
	lightingMaterial.setUniform(
		"_roughness_metallic",
		m_gbufferFB.getAttachment(FrameBufferAttachment::COLOR3)
	);
	lightingMaterial.setUniform("irradiance_map", TextureHandle::IRRADIANCE);

	lightingMaterial.setUniform("LightsData", m_lightsUBO);
}

void RenderPipeline::render(RenderSpecifications& specs) {
	if (!m_shadowmapsGenerated) {
		m_shadowMapFB.bind();
		renderShadowMaps(specs);
		m_shadowmapsGenerated = true;
	}

	if (!m_irradianceGenerated) {
		m_irradianceCompute->dispatch(glm::ivec3(32, 32, 6));
		m_irradianceGenerated = true;
	}

	m_gbufferFB.setResolution(
		specs.resolution, m_resourceManager->getTextureManager()
	);
	m_gbufferFB.bind();

	Frustum frustum = specs.scene.m_camera.getFrustum(
		glm::vec2(0.1, 10000), (float)specs.resolution.x / specs.resolution.y
	);

	ResourceManager& resourceManager = *m_resourceManager;
	std::vector<std::reference_wrapper<Primitive>> opaquePrimitives =
		specs.scene.getPrimitives([frustum,
	                               &resourceManager](Primitive& primitive) {
			const Material& material =
				resourceManager.getMaterial(primitive.getMaterialIndex());
			return frustum.isSphereInFrustum(
				primitive.getPosition(), primitive.getSize()
			);
		});

	RenderPassSpecs simpleRenderPass {
		.primitives = opaquePrimitives,
		.scene = specs.scene,
	};

	renderSubpass(simpleRenderPass);
	m_finalFB.setResolution(
		specs.resolution, m_resourceManager->getTextureManager()
	);
	m_finalFB.bind();

	renderFullscreenPass(m_lightingMaterial);
	renderFullscreenPass(m_skyboxMaterial, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	renderFullscreenPass(m_compositionMaterial);
}

void RenderPipeline::renderSubpass(RenderPassSpecs& renderPassSpecs) {
	if (renderPassSpecs.primitives.size() == 0) return;

	MaterialHandle currentMaterialHandle = MaterialHandle::UNASSIGNED;
	Program* program = nullptr;

	if (renderPassSpecs.overrideMaterial != MaterialHandle::UNASSIGNED) {
		currentMaterialHandle = renderPassSpecs.overrideMaterial;
		program = &m_resourceManager->getProgram(
			m_resourceManager->getMaterial(currentMaterialHandle).getProgram()
		);
		m_resourceManager->getMaterial(currentMaterialHandle)
			.bind(*m_resourceManager);
	}

	for (Primitive& primitive : renderPassSpecs.primitives) {
		if (renderPassSpecs.overrideMaterial == MaterialHandle::UNASSIGNED) {
			MaterialHandle primitiveMaterialHandle =
				primitive.getMaterialIndex();

			if (primitiveMaterialHandle != currentMaterialHandle) {
				currentMaterialHandle = primitiveMaterialHandle;

				Material& material =
					m_resourceManager->getMaterial(currentMaterialHandle);

				material.bind(*m_resourceManager);
				program = &m_resourceManager->getProgram(material.getProgram());
			}
		}

		auto vertexArray = primitive.getVertexArray();
		vertexArray.bind();

		program->setUniform("model", primitive.getTransformationMatrix());

		glDrawElements(
			GL_TRIANGLES,
			vertexArray.getIndexCount(),
			vertexArray.getIndexFormat(),
			0
		);
	}
}

void RenderPipeline::renderShadowMaps(RenderSpecifications& specs) {
	std::vector<Light>& lights = specs.scene.getLights();

	Light::LightUniformBuffer lightUBO { .count = (uint32_t)lights.size() };

	for (int i = 0; i < lights.size(); i++) {
		Light& light = lights[i];

		if (light.getShadowMap() == TextureHandle::UNASSIGNED) {
			TextureManager::TextureSpecification shadowMapSpecs {
				.definition = {
					.format = GL_DEPTH_COMPONENT24,
					.width = 512,
					.height = 512,
				}

			};
			if (light.getType() == Light::Type::Point) {
				shadowMapSpecs.definition.type = GL_TEXTURE_CUBE_MAP;
			}
			TextureHandle shadowMap =
				m_resourceManager->getTextureManager().createTexture(
					shadowMapSpecs
				);
			light.setShadowMap(shadowMap);
		}

		lightUBO.lights[i] = Light::LightUniform {
			.color = glm::vec4(light.getColor(), light.getIntensity()),
			.light_tranformation = light.getTransformationMatrix(true),
			.light_projection = light.getProjectionMatrix(),
			.shadow_map = m_resourceManager->getTextureManager()
			                  .getTexture(light.getShadowMap())
			                  .textureBindlessHandle,
		};
	}

	m_resourceManager->updateUBO(m_lightsUBO, lightUBO);

	Material& shadowmapMaterial =
		m_resourceManager->getMaterial(m_shadowMapMaterial);
	Material& omniShadowmapMaterial =
		m_resourceManager->getMaterial(m_omniShadowMapMaterial);

	for (int i = 0; i < lights.size(); i++) {
		Light& light = lights[i];

		auto primitives = specs.scene.getPrimitives([](Primitive& primitive) {
			return true;
		});

		Material& currentMaterial = light.getType() == Light::Type::Point
		                                ? omniShadowmapMaterial
		                                : shadowmapMaterial;

		currentMaterial.setUniform("current_light", i);
		m_shadowMapFB.setAttachment(
			FrameBufferAttachment::DEPTH,
			light.getShadowMap(),
			m_resourceManager->getTextureManager()
		);

		glClear(GL_DEPTH_BUFFER_BIT);

		RenderPassSpecs shadowMapPass {
			.overrideMaterial = light.getType() != Light::Type::Point
			                        ? m_shadowMapMaterial
			                        : m_omniShadowMapMaterial,
			.primitives = primitives,
			.scene = specs.scene,
		};
		renderSubpass(shadowMapPass);
	}
}

void RenderPipeline::renderFullscreenPass(
	MaterialHandle handle, bool depthTest
) {
	if (m_quadVao == 0) glGenVertexArrays(1, &m_quadVao);

	Material& material = m_resourceManager->getMaterial(handle);
	material.bind(*m_resourceManager);
	if (!depthTest) {
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}
	glBindVertexArray(m_quadVao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	if (!depthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}
}
#include "ShadowPass.h"

#include "Light.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "Scene.h"
#include "glad/glad.h"
#include "renderpipeline/BasePass.h"

ShadowPass::ShadowPass(ResourceManager& resourceManager) {
	m_shadowMapFB = FrameBuffer::getShadowMapFB(
		resourceManager.getTextureManager(), glm::uvec2(512, 512)
	);
	ProgramHandle shadowProgram = resourceManager.registerProgram({
		.vertex = "resources/shaders/shadow.vert",
		.fragment = "resources/shaders/shadow.frag",
	});

	m_shadowMaterial =
		resourceManager.registerMaterial(Material(shadowProgram));
	m_shadowPass = BasePass(m_shadowMaterial);

	ProgramHandle omniShadowProgram = resourceManager.registerProgram({
		.vertex = "resources/shaders/shadow_omni.vert",
		.geometry = "resources/shaders/shadow_omni.geom",
		.fragment = "resources/shaders/shadow.frag",
	});

	m_omniShadowMaterial =
		resourceManager.registerMaterial(Material(omniShadowProgram));
	m_omniShadowPass = BasePass(m_omniShadowMaterial);

	resourceManager.getMaterial(m_shadowMaterial)
		.setUniform("LightsData", UBOHandle::LIGHTS);
	resourceManager.getMaterial(m_omniShadowMaterial)
		.setUniform("LightsData", UBOHandle::LIGHTS);

	auto cubemapTrasformsUBO =
		resourceManager.registerUBO(CubemapTrasforms::get(), 2);
	resourceManager.getMaterial(m_omniShadowMaterial)
		.setUniform("shadow_matrices", cubemapTrasformsUBO);
}

void ShadowPass::render(Scene& scene, ResourceManager& resourceManager) {
	std::vector<Light>& lights = scene.getLights();

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
				resourceManager.getTextureManager().createTexture(shadowMapSpecs
			    );
			light.setShadowMap(shadowMap);
		}

		lightUBO.lights[i] = Light::LightUniform {
			.color = glm::vec4(light.getColor(), light.getIntensity()),
			.light_tranformation = light.getTransformationMatrix(true),
			.light_projection = light.getProjectionMatrix(),
			.shadow_map = resourceManager.getTextureManager()
			                  .getTexture(light.getShadowMap())
			                  .textureBindlessHandle,
		};
	}

	resourceManager.updateUBO(UBOHandle::LIGHTS, lightUBO);

	Material& shadowmapMaterial = resourceManager.getMaterial(m_shadowMaterial);
	Material& omniShadowmapMaterial =
		resourceManager.getMaterial(m_omniShadowMaterial);

	m_shadowMapFB.bind();

	for (int i = 0; i < lights.size(); i++) {
		Light& light = lights[i];

		auto primitives =
			scene.getPrimitives([](Primitive& primitive) { return true; });

		Material& currentMaterial = light.getType() == Light::Type::Point
		                                ? omniShadowmapMaterial
		                                : shadowmapMaterial;

		currentMaterial.setUniform("current_light", i);
		m_shadowMapFB.setAttachment(
			FrameBufferAttachment::DEPTH,
			light.getShadowMap(),
			resourceManager.getTextureManager()
		);

		glClear(GL_DEPTH_BUFFER_BIT);

		if (light.getType() == Light::Type::Point)
			m_omniShadowPass.value().render(primitives, resourceManager);
		else
			m_shadowPass.value().render(primitives, resourceManager);
	}
}
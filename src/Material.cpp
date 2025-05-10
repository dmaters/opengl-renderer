#include "Material.h"

#include <cstdint>

#include "Program.h"
#include "ResourceManager.h"
#include "Resources.h"
#include "TextureManager.h"

void Material::bind(ResourceManager& resourceManager) {
	TextureManager& textureManager = resourceManager.getTextureManager();
	Program& program = resourceManager.getProgram(m_program);
	program.bind();

	for (auto& [name, uniform] : m_uniformValues) {
		std::visit(
			[&name, &resourceManager, &textureManager, &program](
				auto& uniformValue
			) {
				using T = std::decay_t<decltype(uniformValue)>;

				if constexpr (std::is_same_v<T, TextureHandle>) {
					program.setUniform(
						name, textureManager.getTexture(uniformValue)
					);
				} else if constexpr (std::is_same_v<T, UBOHandle>) {
					ResourceManager::UBOData ubo =
						resourceManager.getUBO(uniformValue);
					program.setUBO(name, ubo.id, ubo.binding);
				} else
					program.setUniform(name, uniformValue);
			},
			uniform
		);
	}
}
void Material::bind(ResourceManager& resourceManager, Material& oldMaterial) {
	TextureManager& textureManager = resourceManager.getTextureManager();

	if (m_program != oldMaterial.m_program) bind(resourceManager);

	std::vector<std::string> differences;

	for (auto& [name, value1] : oldMaterial.m_uniformValues) {
		auto it = m_uniformValues.find(name);
		if (it != m_uniformValues.end()) {
			auto value2 = it->second;

			if (value1 != value2) {
				differences.push_back(name);
			}
		}
	}
	Program& program = resourceManager.getProgram(m_program);

	for (std::string& name : differences) {
		std::visit(
			[&name, &resourceManager, &textureManager, &program](
				auto& uniformValue
			) {
				using T = std::decay_t<decltype(uniformValue)>;

				if constexpr (std::is_same_v<T, TextureHandle>) {
					program.setUniform_const(
						name, textureManager.getTexture_const(uniformValue)
					);
				} else if constexpr (std::is_same_v<T, UBOHandle>) {
					ResourceManager::UBOData ubo =
						resourceManager.getUBO(uniformValue);
					program.setUBO(name, ubo.id, ubo.binding);
				} else
					program.setUniform(name, uniformValue);
			},
			m_uniformValues[name]
		);
	}
}

enum class PBRComponents : uint8_t {
	NONE = 0,
	ALBEDO_VALUE = 1 << 0,
	ROUGHNESS_VALUE = 1 << 1,
	METALLIC_VALUE = 1 << 2,
	EMISSION_VALUE = 1 << 3,
};
PBRComponents operator|(PBRComponents a, PBRComponents b) {
	return static_cast<PBRComponents>(
		static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
	);
}

Material Material::StandardPBRMaterial(PBRMaterialValues values) {
	Material material;
	PBRComponents components = PBRComponents::NONE;
	material.m_program = ProgramHandle::GBUFFER;
	// Albedo

	if (values.albedo != TextureHandle::UNASSIGNED)
		material.setUniform("albedo", values.albedo);
	else {
		if (values.albedoColor.has_value()) {
			material.setUniform("albedo_color", values.albedoColor.value());
			components = PBRComponents::ALBEDO_VALUE;
		}
		material.setUniform("albedo", TextureHandle::DEFAULT_ALBEDO);
	}

	// Normal
	if (values.normal != TextureHandle::UNASSIGNED)
		material.setUniform("normal", values.normal);
	else
		material.setUniform("normal", TextureHandle::DEFAULT_NORMAL);

	// Roughness - Metallic

	if (values.roughnessMetallic != TextureHandle::UNASSIGNED)
		material.setUniform("roughness_metallic", values.roughnessMetallic);
	else {
		if (values.roughnessValue.has_value()) {
			material.setUniform(
				"roughness_value", values.roughnessValue.value()
			);
			components = components | PBRComponents::ROUGHNESS_VALUE;
		}
		if (values.metallicValue.has_value()) {
			material.setUniform("metallic_value", values.metallicValue.value());
			components = components | PBRComponents::METALLIC_VALUE;
		}
		material.setUniform(
			"roughness_metallic", TextureHandle::DEFAULT_ROUGHNESS_METALLIC
		);
	}

	// Emissive

	if (values.emission != TextureHandle::UNASSIGNED)
		material.setUniform("emission", values.emission);
	else {
		if (values.emissionValue.has_value()) {
			material.setUniform("emission_value", values.emissionValue.value());
			components = components | PBRComponents::EMISSION_VALUE;
		}
		material.setUniform("emission", TextureHandle::DEFAULT_EMISSION);
	}

	material.setUniform("components", (int)components);
	material.setUniform("projection_view", UBOHandle::PROJECTION_VIEW);
	//	material.setUniform("irradiance_map", TextureHandle::IRRADIANCE);
	return material;
}

Material Material::CustomMaterial(ProgramHandle program)

{
	Material material;

	material.m_program = program;

	return material;
}

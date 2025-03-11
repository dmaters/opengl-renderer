#include "Material.h"

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
			[name, resourceManager, textureManager, &program](auto& uniformValue
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

Material Material::StandardPBRMaterial(
	TextureHandle albedo,
	TextureHandle normal,
	TextureHandle roughnessMetallic,
	TextureHandle emission
) {
	Material material;

	material.m_program = ProgramHandle::FORWARD;
	material.setUniform(
		"albedo",
		albedo != TextureHandle::UNASSIGNED ? albedo
											: TextureHandle::DEFAULT_ALBEDO
	);
	material.setUniform(
		"normal",
		normal != TextureHandle::UNASSIGNED ? normal
											: TextureHandle::DEFAULT_NORMAL
	);

	material.setUniform(
		"roughness_metallic",
		roughnessMetallic != TextureHandle::UNASSIGNED
			? roughnessMetallic
			: TextureHandle::DEFAULT_ROUGHNESS_METALLIC
	);

	material.setUniform(
		"emission",
		emission != TextureHandle::UNASSIGNED ? emission
											  : TextureHandle::DEFAULT_EMISSION
	);
	material.setUniform("projection_view", UBOHandle::PROJECTION_VIEW);

	return material;
}

Material Material::CustomMaterial(ProgramHandle program)

{
	Material material;

	material.m_program = program;

	return material;
}

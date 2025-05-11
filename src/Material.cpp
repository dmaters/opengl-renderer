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

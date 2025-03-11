#include "ResourceManager.h"

#include <cassert>
#include <filesystem>
#include <memory>
#include <utility>

#include "Light.h"
#include "Material.h"
#include "Primitive.h"
#include "Program.h"
#include "Resources.h"
#include "Texture.h"
#include "TextureManager.h"
#include "glad/glad.h"

ResourceManager::ResourceManager() : m_textureManager() {
	registerProgram(
		Program::Stages {
			.vertex = Program::DefaultPrograms::STANDARD_FORWARD::VERTEX,
			.fragment = Program::DefaultPrograms::STANDARD_FORWARD::FRAGMENT },
		ProgramHandle::FORWARD
	);

	registerMaterial(
		MaterialHandle::DEFAULT,
		Material::StandardPBRMaterial(
			TextureHandle::DEFAULT_ALBEDO,
			TextureHandle::DEFAULT_NORMAL,
			TextureHandle::DEFAULT_ROUGHNESS_METALLIC,
			TextureHandle::DEFAULT_EMISSION
		)
	);

	Primitive::ViewProjectionUniformBuffer vpUBO = {};
	registerUBO(UBOHandle::PROJECTION_VIEW, vpUBO, 0);
};

MaterialHandle ResourceManager::registerMaterial(
	MaterialHandle handle, Material material
) {
	m_materials[handle] = material;
	return handle;
}
MaterialHandle ResourceManager::registerMaterial(Material material) {
	MaterialHandle handle { m_nextMaterialHandleValue };
	while (m_materials.contains(handle)) {
		m_nextMaterialHandleValue++;
		handle = { m_nextMaterialHandleValue };
	}

	return registerMaterial(handle, material);

	return handle;
}

ProgramHandle ResourceManager::registerProgram(
	Program::Stages stages, ProgramHandle handle

) {
	if (handle == ProgramHandle::UNASSIGNED) {
		if (m_programCache.find(stages) == m_programCache.end()) {
			while (m_programs.contains({ m_nextProgramHandleValue })) {
				m_nextProgramHandleValue++;
			}

			handle = { m_nextProgramHandleValue };
		} else
			return m_programCache.at(stages);
	}

	m_programCache.emplace(stages, handle);
	m_programs.emplace(handle, Program(stages));

	return handle;
}

#pragma once

#include <filesystem>
#include <map>
#include <tuple>

#include "Material.h"
#include "Program.h"
#include "Resources.h"
#include "TextureManager.h"

class ResourceManager {
public:
	class UBOs;
	struct UBOData {
		GLuint id;
		GLuint binding;
	};

private:
	TextureManager m_textureManager;

	uint32_t m_nextMaterialHandleValue = 1;
	uint32_t m_nextProgramHandleValue = 1;
	uint32_t m_nextUBOHandleValue = 1;

	std::map<MaterialHandle, Material> m_materials;
	std::map<ProgramHandle, Program> m_programs;
	std::map<UBOHandle, UBOData> m_ubos;

	std::map<Program::Stages, ProgramHandle> m_programCache;

	MaterialHandle registerMaterial(MaterialHandle handle, Material material);
	template <typename T>
	UBOHandle registerUBO(UBOHandle handle, T ubo, GLuint binding);

public:
	ResourceManager();

	inline TextureManager& getTextureManager() { return m_textureManager; }

	MaterialHandle registerMaterial(Material material);

	inline Material& getMaterial(MaterialHandle handle) {
		return m_materials.at(handle);
	}
	inline const Material& getMaterial(MaterialHandle handle) const {
		return m_materials.at(handle);
	}

	ProgramHandle registerProgram(
		Program::Stages stages, ProgramHandle handle = ProgramHandle::UNASSIGNED
	);

	inline Program& getProgram(ProgramHandle handle) {
		return m_programs.at(handle);
	}

	template <typename T>
	UBOHandle registerUBO(T ubo, GLuint binding);
	template <typename T>
	UBOHandle updateUBO(UBOHandle handle, T ubo);

	inline UBOData getUBO(UBOHandle handle) const { return m_ubos.at(handle); }
};

template <typename T>
inline UBOHandle ResourceManager::registerUBO(
	UBOHandle handle, T uboData, GLuint binding
) {
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &uboData, GL_DYNAMIC_DRAW);

	m_ubos[handle] = UBOData { ubo, binding };
	return handle;
}
template <typename T>
inline UBOHandle ResourceManager::registerUBO(T uboData, GLuint binding) {
	UBOHandle handle = UBOHandle { m_nextUBOHandleValue };

	while (m_ubos.contains(handle)) {
		m_nextUBOHandleValue++;
		handle = UBOHandle { m_nextUBOHandleValue };
	};

	return registerUBO(handle, uboData, binding);
}
template <typename T>
inline UBOHandle ResourceManager::updateUBO(UBOHandle handle, T ubo) {
	assert(m_ubos.find(handle) != m_ubos.end());

	UBOData uboData = m_ubos[handle];

	glBindBuffer(GL_UNIFORM_BUFFER, uboData.id);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &ubo);

	return handle;
}

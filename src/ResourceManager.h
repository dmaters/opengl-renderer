#pragma once

#include <map>

#include "Material.h"
#include "Program.h"
#include "Resources.h"
#include "TextureManager.h"

struct PBRMaterialValues {
	TextureHandle albedo;
	TextureHandle normal;
	TextureHandle roughnessMetallic;
	TextureHandle emission;
	std::optional<glm::vec4> albedoColor = glm::vec4(0);
	std::optional<float> roughnessValue = 1;
	std::optional<float> metallicValue = 0;
	std::optional<float> emissionValue = 0;
};

class ResourceManager {
public:
	class UBOs;
	struct UBOData {
		GLuint id;
		GLuint binding;
	};

private:
	std::unique_ptr<TextureManager> m_textureManager;

	uint32_t m_nextMaterialHandleValue = 1;
	uint32_t m_nextProgramHandleValue = 1;
	uint32_t m_nextUBOHandleValue = 2;

	uint32_t m_pbrInstances = 0;

	std::map<MaterialHandle, Material> m_materials;
	std::map<ProgramHandle, Program> m_programs;
	std::map<UBOHandle, UBOData> m_ubos;

	std::map<Program::Stages, ProgramHandle> m_programCache;

	GLuint m_quadVao = 0;

	template <typename T>
	UBOHandle registerUBO(UBOHandle handle, T ubo, GLuint binding);

public:
	ResourceManager();

	TextureManager& getTextureManager() { return *m_textureManager; }

	MaterialHandle registerMaterial(
		Material material, MaterialHandle handle = MaterialHandle::UNASSIGNED
	);
	MaterialHandle registerMaterial(
		PBRMaterialValues& values,
		MaterialHandle handle = MaterialHandle::UNASSIGNED
	);

	Material& getMaterial(MaterialHandle handle) {
		return m_materials.at(handle);
	}
	const Material& getMaterial(MaterialHandle handle) const {
		return m_materials.at(handle);
	}

	ProgramHandle registerProgram(
		Program::Stages stages, ProgramHandle handle = ProgramHandle::UNASSIGNED
	);

	Program& getProgram(ProgramHandle handle) { return m_programs.at(handle); }

	template <typename T>
	UBOHandle registerUBO(T ubo, GLuint binding);
	template <typename T>
	UBOHandle updateUBO(UBOHandle handle, T ubo);

	UBOData getUBO(UBOHandle handle) const { return m_ubos.at(handle); }

	GLuint getQuadVAO() {
		if (m_quadVao == 0) glGenVertexArrays(1, &m_quadVao);
		return m_quadVao;
	}
};

template <typename T>
UBOHandle ResourceManager::registerUBO(
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
UBOHandle ResourceManager::registerUBO(T uboData, GLuint binding) {
	UBOHandle handle = UBOHandle { m_nextUBOHandleValue };

	while (m_ubos.contains(handle)) {
		m_nextUBOHandleValue++;
		handle = UBOHandle { m_nextUBOHandleValue };
	};

	return registerUBO(handle, uboData, binding);
}
template <typename T>
UBOHandle ResourceManager::updateUBO(UBOHandle handle, T ubo) {
	assert(m_ubos.find(handle) != m_ubos.end());

	UBOData uboData = m_ubos[handle];

	glBindBuffer(GL_UNIFORM_BUFFER, uboData.id);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &ubo);

	return handle;
}

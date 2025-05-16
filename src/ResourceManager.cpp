#include "ResourceManager.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
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
#include "glm/ext/vector_float4.hpp"

struct alignas(16) PBRMaterialInstance {
	uint32_t albedo = 0;
	uint32_t normal = 0;
	uint32_t metallicRoughness = 0;
	uint32_t components = 0;
	glm::vec4 albedo_color = glm::vec4(0);
	float roughness_value = 0;
	float metallic_value = 0;
	uint32_t _padding = 0;
};

struct PBRMaterialInstancesUBO {
	PBRMaterialInstance instances[128];
};

ResourceManager::ResourceManager() {
	registerProgram(
		Program::Stages { .vertex = "resources/shaders/gbuffer.vert",
	                      .fragment = "resources/shaders/gbuffer.frag" },
		ProgramHandle::GBUFFER
	);
	PBRMaterialValues defaultInstance {
		TextureHandle::DEFAULT_ALBEDO,
		TextureHandle::DEFAULT_NORMAL,
		TextureHandle::DEFAULT_ROUGHNESS_METALLIC,
		TextureHandle::DEFAULT_EMISSION,
	};

	Primitive::ViewProjectionUniformBuffer vpUBO = {};
	registerUBO(UBOHandle::PROJECTION_VIEW, vpUBO, 0);

	std::array<uint64_t, MAX_TEXTURE_COUNT> textures = {};
	registerUBO(UBOHandle::TEXTURES, textures, 1);
	registerUBO(UBOHandle::PBR_INSTANCES, PBRMaterialInstancesUBO {}, 2);
	registerUBO(UBOHandle::LIGHTS, Light::LightUniformBuffer {}, 3);

	registerMaterial(defaultInstance, MaterialHandle::DEFAULT);

	m_textureManager =
		std::make_unique<TextureManager>(getUBO(UBOHandle::TEXTURES).id);
};

MaterialHandle ResourceManager::registerMaterial(
	Material material, MaterialHandle handle
) {
	if (handle != MaterialHandle::UNASSIGNED) {
		m_materials[handle] = material;
		return handle;
	}

	handle = { m_nextMaterialHandleValue };
	while (m_materials.contains(handle)) {
		m_nextMaterialHandleValue++;
		handle = { m_nextMaterialHandleValue };
	}
	m_materials[handle] = material;
	return handle;
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

PBRMaterialInstance valueToInstance(PBRMaterialValues& values) {
	PBRMaterialInstance instance;
	PBRComponents components = PBRComponents::NONE;
	// Albedo

	if (values.albedo != TextureHandle::UNASSIGNED) {
		instance.albedo = values.albedo.value;
	} else {
		if (values.albedoColor.has_value()) {
			instance.albedo_color = values.albedoColor.value();
			components = PBRComponents::ALBEDO_VALUE;
		}
		instance.albedo = TextureHandle::DEFAULT_ALBEDO.value;
	}

	// Normal
	instance.normal = values.normal != TextureHandle::UNASSIGNED
	                      ? values.normal.value
	                      : TextureHandle::DEFAULT_NORMAL.value;

	// Metallic Roughness

	if (values.metallicRoughness != TextureHandle::UNASSIGNED)
		instance.metallicRoughness = values.metallicRoughness.value;
	else {
		if (values.roughnessValue.has_value()) {
			instance.roughness_value = values.roughnessValue.value();
			components = components | PBRComponents::ROUGHNESS_VALUE;
		}
		if (values.metallicValue.has_value()) {
			instance.metallic_value = values.metallicValue.value();
			components = components | PBRComponents::METALLIC_VALUE;
		}
		instance.metallicRoughness =
			TextureHandle::DEFAULT_ROUGHNESS_METALLIC.value;
	}

	// Emissive
	/*
	if (values.emission != TextureHandle::UNASSIGNED)
	instance.emission = values.emission.value;
	else {
	    if (values.emissionValue.has_value()) {
	        instance.emission_value = values.emissionValue.value();
	        components = components | PBRComponents::EMISSION_VALUE;
	    }
	    instance.emission = TextureHandle::DEFAULT_EMISSION.value;
	}
	*/

	instance.components = (int32_t)components;
	return instance;
}

MaterialHandle ResourceManager::registerMaterial(
	PBRMaterialValues& values, MaterialHandle handle
) {
	assert(m_pbrInstances < 128);

	Material material = Material(ProgramHandle::GBUFFER);
	PBRMaterialInstance instance = valueToInstance(values);

	UBOData ubo = getUBO(UBOHandle::PBR_INSTANCES);

	glNamedBufferSubData(
		ubo.id,
		sizeof(PBRMaterialInstance) * m_pbrInstances,
		sizeof(PBRMaterialInstance),
		&instance
	);

	material.setUniform("material_instances", UBOHandle::PBR_INSTANCES);
	material.setUniform("_textures", UBOHandle::TEXTURES);

	material.setUniform("instance_index", (int32_t)m_pbrInstances);
	material.setUniform("projection_view", UBOHandle::PROJECTION_VIEW);
	m_pbrInstances++;

	return registerMaterial(material, handle);
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

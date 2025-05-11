#pragma once

#include <glad/glad.h>

#include <cassert>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

#include "Program.h"
#include "Resources.h"
#include "glm/ext/matrix_float3x3.hpp"

template <typename T>
concept UniformDataValueType =
	std::same_as<T, float> || std::same_as<T, int> ||
	std::same_as<T, glm::vec2> || std::same_as<T, glm::vec3> ||
	std::same_as<T, glm::vec4> || std::same_as<T, glm::mat3> ||
	std::same_as<T, glm::mat4> || std::same_as<T, TextureHandle> ||
	std::same_as<T, UBOHandle>;
;

class ResourceManager;
class Material {
public:
	using UniformDataValue = std::variant<
		float,
		int,
		glm::vec2,
		glm::vec3,
		glm::vec4,
		glm::mat3,
		glm::mat4,
		TextureHandle,
		UBOHandle>;

private:
	// Using type traits to distinguish between the types
	template <typename T>
	struct is_ubo : std::false_type {};

	template <>
	struct is_ubo<UBOHandle> : std::true_type {};

	template <typename T>
	struct is_texture : std::false_type {};

	template <>
	struct is_texture<TextureHandle> : std::true_type {};

	std::unordered_map<std::string, UniformDataValue> m_uniformValues;
	ProgramHandle m_program = ProgramHandle::UNASSIGNED;
	bool m_trasparency = false;

public:
	Material() {}
	Material(ProgramHandle program) : m_program(program) {}

	void bind(ResourceManager& allocator);
	void bind(ResourceManager& allocator, Material& oldMaterial);

	template <UniformDataValueType T>
	void setUniform(std::string name, T uniform) {
		m_uniformValues[name] = UniformDataValue(uniform);
	}

	template <UniformDataValueType T>
	T getUniform(std::string name) {
		assert(m_uniformValues.contains(name));
		return m_uniformValues.at(name);
	}

	inline ProgramHandle getProgram() const { return m_program; }

	inline void setTrasparencyFlag(bool trasparency) {
		m_trasparency = trasparency;
	}
	bool getTrasparencyFlag() const { return m_trasparency; }
};

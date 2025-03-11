#pragma once

#include <glad/glad.h>

#include <map>
#include <string>
#include <variant>

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
	static Material StandardPBRMaterial(

		TextureHandle albedo,
		TextureHandle normal,
		TextureHandle roughnessMetallic,
		TextureHandle emission

	);

	static Material CustomMaterial(ProgramHandle program);

private:
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

	// Using type traits to distinguish between the types
	template <typename T>
	struct is_ubo : std::false_type {};

	template <>
	struct is_ubo<UBOHandle> : std::true_type {};

	template <typename T>
	struct is_texture : std::false_type {};

	template <>
	struct is_texture<TextureHandle> : std::true_type {};

	std::map<std::string, UniformDataValue> m_uniformValues;
	ProgramHandle m_program;

public:
	void bind(ResourceManager& allocator);
	void bind(ResourceManager& allocator, Material& oldMaterial);

	template <UniformDataValueType T>
	void setUniform(std::string name, T uniform) {
		m_uniformValues[name] = UniformDataValue(uniform);
	}

	template <UniformDataValueType T>
	T getUniform(std::string name) {
		return m_uniformValues[name];
	}

	inline ProgramHandle getProgram() const { return m_program; }
};

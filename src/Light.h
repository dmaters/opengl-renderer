#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include <numbers>

#include "Node.h"
#include "Resources.h"
#include "glm/fwd.hpp"

class Light : public Node {
public:
	struct LightUniform;
	struct LightUniformBuffer;

	enum class Type {
		Directional,
		Point,
		Spot
	};

private:
	friend class LightComponent;
	Type m_type = Type::Directional;
	glm::vec3 m_color = glm::vec3(1.0f);
	float m_intensity = 1.0f;
	TextureHandle m_shadowMap = TextureHandle::UNASSIGNED;
	float m_falloff = 0.0;

public:
	Light(Type type) : m_type(type) {}
	inline void setColor(glm::vec3 color) { m_color = color; }
	inline glm::vec3 getColor() const { return m_color; }
	inline void setIntensity(float intensity) { m_intensity = intensity; }
	inline float getIntensity() const { return m_intensity; }

	inline float getFalloff() const { return m_falloff; }
	inline void setFalloff(float falloff) { m_falloff = falloff; }

	inline TextureHandle getShadowMap() const { return m_shadowMap; }
	inline void setShadowMap(TextureHandle shadowMap) {
		m_shadowMap = shadowMap;
	}

	inline Type getType() const { return m_type; }

	glm::mat4 getProjectionMatrix() {
		switch (m_type) {
			case Type::Directional:
				return glm::ortho(
					-m_falloff / 2,
					m_falloff / 2,
					-m_falloff / 2,
					m_falloff / 2,
					0.1f,
					m_falloff
				);
			case Type::Point:
				return glm::perspective(
					glm::radians(90.0f), 1.0f, 0.1f, 1500.0f
				);
			case Type::Spot:
				return glm::perspective(
					glm::radians(45.0f), 1.0f, 0.1f, 1500.0f
				);
			default:
				return glm::perspective(
					glm::radians(45.0f), 1.0f, 0.1f, 1500.0f
				);
		}
	};
};

struct Light::LightUniform {
	glm::vec4 color;
	glm::mat4 light_tranformation;
	glm::mat4 light_projection;
	glm::uint64_t shadow_map;
	float _padding[2];
};
struct Light::LightUniformBuffer {
	uint32_t count = 0;
	uint32_t scene_light = 0;
	float _padding[2];
	LightUniform lights[2];
};

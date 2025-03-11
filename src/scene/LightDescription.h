#pragma once
#include <glm/glm.hpp>

struct LightDescription {
	enum class Type {
		Directional,
		Point,
		Spot
	};

	Type type;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	float intensity;
};
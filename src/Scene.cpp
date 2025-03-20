#include "Scene.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "Light.h"
#include "Node.h"
#include "Primitive.h"
#include "glm/ext/matrix_float3x3.hpp"
#include "scene/LightDescription.h"

void Scene::addLight(LightDescription& desc) {
	Light light((Light::Type)desc.type);

	light.setColor(desc.color);
	light.setIntensity(desc.intensity);
	light.setPosition(desc.position);

	glm::vec3 right, up;

	if (abs(glm::dot(desc.direction, glm::vec3(0, 1, 0))) == 1) {
		up = glm::vec3(0.0f, 0.0f, desc.direction.y);
		right = glm::normalize(glm::cross(up, desc.direction));

	} else {
		right =
			glm::normalize(glm::vec3(desc.direction.z, 0, desc.direction.x));
		up = glm::normalize(glm::cross(right, desc.direction));
	}
	light.m_orientation = glm::mat3(right, up, desc.direction);

	if (light.getType() == Light::Type::Directional) {
		light.m_position = -desc.direction * m_bounds;
		light.setFalloff(m_bounds * 2);
	}
	if (light.getType() == Light::Type::Point)
		light.m_orientation = glm::mat3(1);

	m_lights.push_back(light);
}

const std::vector<std::reference_wrapper<Primitive>> Scene::getPrimitives(
	std::function<bool(Primitive&)> cullingFunction
) {
	std::vector<std::reference_wrapper<Primitive>> primitives;

	for (Primitive& primitive : m_primitives) {
		if (cullingFunction(primitive)) primitives.push_back(primitive);
	}
	std::sort(
		primitives.begin(),
		primitives.end(),
		[](std::reference_wrapper<Primitive> a,
	       std::reference_wrapper<Primitive> b) {
			return a.get().getMaterialIndex() < b.get().getMaterialIndex();
		}
	);
	return primitives;
}
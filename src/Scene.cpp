#include "Scene.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <vector>

#include "Frustum.hpp"
#include "Light.h"
#include "Node.h"
#include "Primitive.h"
#include "scene/LightDescription.h"

void Scene::addLight(LightDescription& desc) {
	Light light((Light::Type)desc.type);

	light.setColor(desc.color);
	light.setIntensity(desc.intensity);
	light.setPosition(desc.position);

	if (desc.direction == glm::vec3(0.0f, -1.0f, 0.0f) ||
	    desc.direction == glm::vec3(0.0f, 1.0f, 0.0f)) {
		light.m_orientation = glm::mat4_cast(glm::angleAxis(
			glm::sign(desc.direction.y) * glm::pi<float>() / 2,
			glm::vec3(1.0f, 0.0f, 0.0f)
		));
	} else if (desc.direction != glm::vec3(0.0f, 0.0f, 0.0f)) {
		light.m_orientation = glm::lookAt(
			desc.position, desc.position + desc.direction, glm::vec3(0, 1, 0)
		);
	}

	if (light.getType() == Light::Type::Directional) {
		light.setFalloff(m_highBoundHeight - m_lowBoundHeight);
		desc.position.y = m_highBoundHeight;
		light.setPosition(desc.position);
	}

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
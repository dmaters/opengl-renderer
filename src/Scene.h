#pragma once

#include <cassert>
#include <functional>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "Camera.h"
#include "Frustum.hpp"
#include "Light.h"
#include "Node.h"
#include "Primitive.h"
#include "scene/LightDescription.h"

class Scene {
public:
	float m_lowBoundHeight = 0.f;
	float m_highBoundHeight = 0.f;

	std::vector<Light> m_lights;
	std::vector<Primitive> m_primitives;

	Camera m_camera;

	friend class SceneLoader;

public:
	Scene();
	inline Camera& getCamera() { return m_camera; }
	void addLight(LightDescription& desc);

	const std::vector<std::reference_wrapper<Primitive>> getPrimitives(
		std::function<bool(Primitive&)> cullingFunction
	);
	std::vector<Light>& getLights() { return m_lights; }
};

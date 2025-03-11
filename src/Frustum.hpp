#pragma once

#include <glm/glm.hpp>

class Frustum {
private:
	enum class Plane {
		LEFT = 0,
		RIGHT = 1,
		BOTTOM = 2,
		TOP = 3,
		NEAR = 4,
		FAR = 5
	};
	glm::vec4 m_planes[6];
	glm::vec3 m_position = glm::vec3(0.0f);

public:
	Frustum(
		glm::vec3 position,
		glm::vec3 direction,
		glm::vec2 nearFarPlanes,
		float fov,
		float aspectRatio
	);
	bool isSphereInFrustum(glm::vec3 position, float radius) const;
};

#pragma once

#include <glm/gtc/quaternion.hpp>

#include "Frustum.hpp"
#include "Node.h"
#include "glm/ext/vector_float3.hpp"

class Camera : public Node {
private:
	float m_fov = 45.0f;
	float m_pitch = 0;
	float m_yaw = 0;
	bool m_speeding = false;
	glm::mat3 m_baseOrientation;

public:
	void movementInput(glm::vec3 direction, float delta);
	void rotationInput(glm::vec2 rotation, float delta);
	inline const Frustum getFrustum(
		glm::vec2 nearFarPlanes, float aspectRatio
	) {
		return Frustum(
			m_position, m_orientation[2], nearFarPlanes, m_fov, aspectRatio
		);
	}
	inline void setOrientation(glm::mat3 orientation) {
		m_baseOrientation = orientation;
	}
};
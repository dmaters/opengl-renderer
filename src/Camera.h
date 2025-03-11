#pragma once

#include <glm/gtc/quaternion.hpp>

#include "Frustum.hpp"
#include "Node.h"
#include "glm/ext/vector_float3.hpp"

class Camera : public Node {
public:
	struct CameraUBO;

private:
	float m_fov = 45.0f;
	float m_pitch = 0;
	float m_yaw = 0;
	bool m_speeding = false;

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
};

struct Camera::CameraUBO {
	glm::vec4 position = glm::vec4(0.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
};

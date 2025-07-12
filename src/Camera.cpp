#include "Camera.h"

#include <cassert>
#include <cmath>

#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

void Camera::movementInput(glm::vec3 direction, float delta) {
	glm::mat3 orientation = getOrientation();
	orientation[2] = -orientation[2];
	glm::vec3 forward = orientation * direction;

	m_position += forward * delta * (!m_speeding ? 100.0f : 300.0f);
}

void Camera::rotationInput(glm::vec2 rotation, float delta) {
	float dPitch = rotation.y * 5;
	if (glm::degrees(abs(m_pitch + dPitch)) > 89) dPitch = 0;
	m_pitch += dPitch;
	float dYaw = rotation.x * 5;
	m_yaw += dYaw;
	glm::quat pitchQuat = glm::angleAxis(m_pitch, glm::vec3(1, 0, 0));
	glm::quat yawQuat = glm::angleAxis(m_yaw, glm::vec3(0, 1, 0));

	m_orientation =
		m_baseOrientation * glm::transpose(glm::mat3_cast(pitchQuat * yawQuat));
}
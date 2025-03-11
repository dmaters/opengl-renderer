#include "Camera.h"

#include "Node.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

void Camera::movementInput(glm::vec3 direction, float delta) {
	glm::vec3 forward = getOrientation() * direction;

	glm::vec3 position =
		getPosition() + forward * delta * (!m_speeding ? 100.0f : 300.0f);
	setPosition(position);
}
void Camera::rotationInput(glm::vec2 rotation, float delta) {
	float dPitch = rotation.y * delta * 1.0f;
	m_pitch += dPitch;
	float dYaw = rotation.x * delta * 1.0f;
	m_yaw += dYaw;

	glm::vec3 right = -glm::vec3(getOrientation()[0]);
	glm::vec3 up = -glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), dYaw, up);
	glm::mat4 pitchMatrix = glm::mat4(1);
	if (m_pitch < glm::radians(89.0f) && m_pitch > glm::radians(-89.0f))
		pitchMatrix = glm::rotate(glm::mat4(1.0f), dPitch, right);

	setOrientation((yawMatrix * pitchMatrix * glm::mat4(getOrientation())));
}

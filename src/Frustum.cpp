#include "Frustum.hpp"

#include <cmath>

#include "glm/common.hpp"
#include "glm/trigonometric.hpp"

// Frustum::Frustum(
// 	glm::vec3 position,
// 	glm::vec3 direction,
// 	glm::vec2 nearFarPlanes,
// 	float fovDeg,
// 	float aspectRatio
// ) {
// 	glm::vec3 right = glm::normalize(glm::vec3(direction.z, 0, direction.x));
// 	glm::vec3 up = glm::normalize(glm::cross(direction, right));

// 	float tanFov = std::tan(glm::radians(fovDeg) * 0.5f);
// 	float nearHeight = nearFarPlanes.x * tanFov;
// 	float nearWidth = nearHeight * (aspectRatio);

// 	m_planes[static_cast<int>(Frustum::Plane::NEAR)] =
// 		glm::vec4(-direction, nearFarPlanes.x);
// 	m_planes[static_cast<int>(Frustum::Plane::FAR)] =
// 		glm::vec4((direction), nearFarPlanes.y);

// 	glm::vec3 leftNormal =
// 		glm::normalize(glm::cross(up, direction - right * nearWidth));
// 	m_planes[static_cast<int>(Frustum::Plane::LEFT)] = glm::vec4(leftNormal, 0);

// 	glm::vec3 rightNormal =
// 		glm::normalize(glm::cross(up, direction + right * nearWidth));
// 	m_planes[static_cast<int>(Frustum::Plane::RIGHT)] =
// 		glm::vec4(-rightNormal, 0);

// 	glm::vec3 topNormal =
// 		glm::normalize(glm::cross(right, direction + up * nearHeight));
// 	m_planes[static_cast<int>(Frustum::Plane::TOP)] = glm::vec4(topNormal, 0);

// 	glm::vec3 bottomNormal =
// 		glm::normalize(glm::cross(right, direction - up * nearHeight));
// 	m_planes[static_cast<int>(Frustum::Plane::BOTTOM)] =
// 		glm::vec4(bottomNormal, 0);

// 	m_position = position;
// };

// bool Frustum::isSphereInFrustum(glm::vec3 spherePos, float radius) const {
// 	for (int i = 0; i < 6; i++) {
// 		float dist =
// 			glm::dot((spherePos - m_position), glm::vec3(m_planes[i])) -
// 			m_planes[i].w;

// 		if (dist - radius > 0) {
// 			return false;
// 		}
// 	}
// 	return true;
// }

Frustum::Frustum(
	glm::vec3 position,
	glm::vec3 direction,
	glm::vec2 nearFarPlanes,
	float fovDeg,
	float aspectRatio
) {
	glm::vec3 right = glm::normalize(glm::vec3(direction.z, 0, direction.x));
	glm::vec3 up = glm::normalize(glm::cross(right, direction));

	float vertical = std::sin(glm::radians(fovDeg * 0.5));
	float horizontal = vertical * aspectRatio;

	// Near and Far planes
	m_planes[static_cast<int>(Frustum::Plane::NEAR)] =
		glm::vec4(-direction, nearFarPlanes.x);
	m_planes[static_cast<int>(Frustum::Plane::FAR)] =
		glm::vec4(direction, nearFarPlanes.y);

	// Left and Right planes
	glm::vec3 leftNormal =
		glm::normalize(glm::cross(-up, direction + right * horizontal));
	m_planes[static_cast<int>(Frustum::Plane::LEFT)] = glm::vec4(leftNormal, 0);

	glm::vec3 rightNormal =
		glm::normalize(glm::cross(up, direction - right * horizontal));
	m_planes[static_cast<int>(Frustum::Plane::RIGHT)] =
		glm::vec4(rightNormal, 0);

	// Top and Bottom planes
	glm::vec3 topNormal =
		glm::normalize(glm::cross(-right, direction - up * vertical));
	m_planes[static_cast<int>(Frustum::Plane::TOP)] = glm::vec4(topNormal, 0);

	glm::vec3 bottomNormal =
		glm::normalize(glm::cross(right, direction + up * vertical));
	m_planes[static_cast<int>(Frustum::Plane::BOTTOM)] =
		glm::vec4(bottomNormal, 0);

	m_position = position;
}

bool Frustum::isSphereInFrustum(glm::vec3 spherePos, float radius) const {
	for (int i = 0; i < 6; i++) {
		float dist =
			glm::dot((spherePos - m_position), glm::vec3(m_planes[i])) -
			m_planes[i].w;
		if (dist - radius > 0) {
			return false;
		}
	}
	return true;
}
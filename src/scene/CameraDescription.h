#pragma once

#include <glm/glm.hpp>

struct CameraDescription {
	glm::vec3 position;
	float fov = 45.0f;
	float pitch = 0;
	float yaw = 0;
};
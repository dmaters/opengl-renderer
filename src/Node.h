#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"

class Node {
protected:
	friend class Scene;
	friend class SceneLoader;

	glm::vec3 m_position = glm::vec3(0);
	glm::vec3 m_scale = glm::vec3(1);
	glm::mat3 m_orientation = glm::mat3(1);

public:
	inline glm::vec3 getPosition() const { return m_position; }
	inline void setPosition(glm::vec3 position) { m_position = position; }

	inline glm::vec3 getScale() const { return m_scale; }
	inline void setScale(glm::vec3 scale) { m_scale = scale; }

	inline glm::mat3 getOrientation() const { return m_orientation; }
	inline void setOrientation(glm::mat3 orientation) {
		m_orientation = orientation;
	}

	inline glm::mat4 getTransformationMatrix(bool viewMatrix = false) const {
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1), m_scale);
		glm::mat4 rotationMatrix = glm::mat4(m_orientation);
		glm::mat4 translation = glm::translate(glm::mat4(1), m_position);
		if (viewMatrix) {
			return glm::inverse(translation * rotationMatrix);
		}
		return translation * rotationMatrix * scaleMatrix;
	}
};
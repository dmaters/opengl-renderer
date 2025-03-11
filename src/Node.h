#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm/fwd.hpp"

class Node {
protected:
	friend class Scene;
	friend class SceneLoader;

	glm::vec3 m_position = glm::vec3(0);
	glm::vec3 m_scale = glm::vec3(1);
	glm::mat3 m_orientation = glm::mat3(1);

public:
	inline glm::vec3 getPosition() const { return m_position; }
	inline glm::vec3 getScale() const { return m_scale; }
	inline virtual glm::mat3 getOrientation() const { return m_orientation; }

	void setPosition(glm::vec3 position) { m_position = position; }
	void setScale(glm::vec3 scale) { m_scale = scale; }
	void setOrientation(glm::mat3 orientation) { m_orientation = orientation; }

	inline glm::mat4 getTransformationMatrix() const {
		glm::mat4 viewMatrix = glm::lookAt(
			m_position, m_position + m_orientation[2], m_orientation[1]
		);

		return viewMatrix;
	}
};
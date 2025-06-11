#pragma once

#include <vector>

#include "Primitive.h"
#include "ResourceManager.h"
#include "Resources.h"

class BasePass {
	MaterialHandle m_material = MaterialHandle::UNASSIGNED;

public:
	BasePass(MaterialHandle material) : m_material(material) {}

	void render(
		const std::vector<std::reference_wrapper<Primitive>>& primitives,
		ResourceManager& resourceManager
	);

	MaterialHandle getMaterial() const { return m_material; }
};

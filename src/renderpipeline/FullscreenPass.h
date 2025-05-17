#pragma once

#include "ResourceManager.h"
#include "Resources.h"
class FullscreenPass {
private:
	MaterialHandle m_material;
	bool m_depthEnabled = false;

public:
	FullscreenPass(MaterialHandle material) : m_material(material) {}

	void render(ResourceManager& resourceManager);
};
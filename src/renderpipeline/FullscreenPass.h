#pragma once

#include "ResourceManager.h"
#include "Resources.h"
class FullscreenPass {
private:
	MaterialHandle m_material;
	bool m_depthEnabled = false;

public:
	FullscreenPass(MaterialHandle material, bool depthEnabled = false) :
		m_material(material), m_depthEnabled(depthEnabled) {}

	void render(ResourceManager& resourceManager);
};
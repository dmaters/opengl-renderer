#pragma once
#include "Renderer.h"
class RendererComponent {
	bool material_tab;

public:
	virtual void Render(Renderer& p_renderer);
};
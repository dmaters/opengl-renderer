#pragma once
#include "Light.h"

class LightComponent {
public:
	static bool Render(Light& light, Node& transform);
};
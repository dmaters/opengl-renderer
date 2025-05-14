#pragma once

#include "FrameBuffer.h"
#include "Node.h"
#include "Primitive.h"
#include "Resources.h"
#include "Scene.h"

struct RenderPassSpecs {
	MaterialHandle overrideMaterial = MaterialHandle::UNASSIGNED;
	bool clear = false;
	const std::vector<std::reference_wrapper<Primitive>>& primitives;
	Scene& scene;
};
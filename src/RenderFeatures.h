#pragma once

enum class RenderFeatures {
	LIGHTING = 1 << 0,
	SHADOWS = 1 << 1,
	SKYBOX = 1 << 2,
	BLOOM = 1 << 3,
};
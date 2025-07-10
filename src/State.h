#pragma once

#include <glad/glad.h>

#include <array>
#include <queue>

struct State {
	std::array<float, 128> frameTimesCPU;
	std::array<float, 128> frameTimesGPU;
	float averageTimeCPU = 0;
	float averageTimeGPU = 0;

	uint64_t frameIndex = 0;

	int primitiveCount = 0;

	static State& Get() {
		static State instance;
		return instance;
	}
};
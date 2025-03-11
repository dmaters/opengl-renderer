#pragma once
#include <glm/glm.hpp>
#include <string>

struct EnvironmentDescription {
	std::string skybox = "/resources/textures/skybox";
	glm::vec3 giColor = glm::vec3(1.0, 1.0, 1.0);
	float giIntensity = 1;

	struct PostProcessingEffects {
		bool bloom = false;
	};

	PostProcessingEffects effects;
};
#pragma once

#include <filesystem>

struct MaterialDescription {
	static bool Compare(
		MaterialDescription material1, MaterialDescription material2
	);
};

struct StandardMaterialDescription : public MaterialDescription {
	std::optional<std::filesystem::path> albedo;
	std::optional<std::filesystem::path> normal;
	std::optional<std::filesystem::path> roughness;
	std::optional<std::filesystem::path> specular;
	std::optional<std::filesystem::path> metallic;
	std::optional<std::filesystem::path> emission;
	std::optional<std::filesystem::path> displacement;

	std::optional<std::filesystem::path> opacity;

	bool trasparency;
};

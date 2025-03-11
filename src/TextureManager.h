#pragma once
#include <glad/glad.h>

#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include "Resources.h"
#include "Texture.h"

class TextureManager {
public:
	struct TextureSpecification;
	struct TextureData;

private:
	uint32_t m_nextHandle = 1;

	std::map<TextureHandle, Texture> m_textures;

	std::map<std::filesystem::path, TextureHandle> m_localTextureCache;

	void setupDefaultTextures();

public:
	TextureManager();

	TextureHandle createTexture(
		const TextureSpecification& specs,
		TextureHandle override = TextureHandle::UNASSIGNED
	);
	TextureHandle loadTexture(
		const std::filesystem::path& path,
		TextureHandle override = TextureHandle::UNASSIGNED
	);

	inline Texture& getTexture(TextureHandle handle) {
		return m_textures[handle];
	}
	inline const Texture& getTexture_const(TextureHandle handle) const {
		return m_textures.at(handle);
	}
	void releaseTexture(TextureHandle handle);
};
struct TextureManager::TextureData {
	GLenum format;
	GLenum encoding;
	std::vector<unsigned char>& data;
};
struct TextureManager::TextureSpecification {
	Texture definition;
	std::optional<TextureData> data;
};

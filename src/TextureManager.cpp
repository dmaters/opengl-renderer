#include "TextureManager.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>

#include "Resources.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
using namespace std::literals::string_view_literals;
TextureManager::TextureManager(GLuint textureUBO) : m_textureUBO(textureUBO) {
	setupDefaultTextures();
}
constexpr std::array CUBEMAP_FACES = { "right.png"sv, "left.png"sv,
	                                   "top.png"sv,   "bottom.png"sv,
	                                   "front.png"sv, "back.png"sv };

TextureHandle TextureManager::createTexture(
	const TextureSpecification& specs, TextureHandle overrideHandle
) {
	Texture texture = specs.definition;

	glCreateTextures(specs.definition.type, 1, &texture.textureID);

	if (specs.definition.depth > 1 &&
	    specs.definition.type != GL_TEXTURE_CUBE_MAP)
		glTextureStorage3D(
			texture.textureID,
			0,
			texture.format,
			texture.width,
			texture.height,
			texture.depth
		);

	else
		glTextureStorage2D(
			texture.textureID,
			texture.mipmap_levels,
			texture.format,
			texture.width,
			texture.height
		);

	glTextureParameteri(texture.textureID, GL_TEXTURE_WRAP_S, texture.wrapping);
	glTextureParameteri(texture.textureID, GL_TEXTURE_WRAP_T, texture.wrapping);
	if (texture.type == GL_TEXTURE_CUBE_MAP)
		glTextureParameteri(
			texture.textureID, GL_TEXTURE_WRAP_R, texture.wrapping
		);

	glTextureParameteri(
		texture.textureID,
		GL_TEXTURE_MIN_FILTER,
		texture.mipmap_levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : texture.filtering
	);
	glTextureParameteri(
		texture.textureID, GL_TEXTURE_MAG_FILTER, texture.filtering
	);

	if (texture.format == GL_DEPTH_COMPONENT24) {
		glTextureParameteri(
			texture.textureID,
			GL_TEXTURE_COMPARE_MODE,
			GL_COMPARE_REF_TO_TEXTURE
		);
	}

	if (specs.data.has_value()) {
		if (texture.type == GL_TEXTURE_CUBE_MAP) {
			size_t faceSize = texture.width * texture.height * 4;  // 4 CHANNELS

			for (int i = 0; i < 6; i++)

			{
				glTextureSubImage3D(
					texture.textureID,
					0,
					0,
					0,
					i,
					texture.width,
					texture.height,
					texture.depth,
					specs.data->format,
					specs.data->encoding,
					specs.data->data.data() + i * faceSize
				);
			}

		} else if (texture.depth > 1)
			glTextureSubImage3D(
				texture.textureID,
				0,
				0,
				0,
				0,
				texture.width,
				texture.height,
				texture.depth,
				specs.data->format,
				specs.data->encoding,
				specs.data->data.data()
			);
		else
			glTextureSubImage2D(
				texture.textureID,
				0,
				0,
				0,
				texture.width,
				texture.height,
				specs.data->format,
				specs.data->encoding,
				specs.data->data.data()
			);
	}
	if (texture.mipmap_levels > 1) glGenerateTextureMipmap(texture.textureID);

	GLuint64 textureHandle = glGetTextureHandleARB(texture.textureID);
	glMakeTextureHandleResidentARB(textureHandle);
	texture.textureBindlessHandle = textureHandle;

	if (overrideHandle == TextureHandle::UNASSIGNED) {
		TextureHandle handle { m_nextHandle };
		m_textures[handle] = texture;
		m_nextHandle++;
		glNamedBufferSubData(
			m_textureUBO,
			sizeof(uint64_t) * handle.value,
			sizeof(uint64_t),
			&textureHandle
		);

		return handle;
	}

	if (m_nextHandle <= overrideHandle.value) {
		m_nextHandle = overrideHandle.value + 1;
	}

	releaseTexture(overrideHandle);

	m_textures[overrideHandle] = texture;

	glNamedBufferSubData(
		m_textureUBO,
		sizeof(uint64_t) * overrideHandle.value,
		sizeof(uint64_t),
		&textureHandle
	);

	return overrideHandle;
}

void TextureManager::releaseTexture(TextureHandle handle) {
	uint32_t textureID = m_textures[handle].textureID;
	m_textures.erase(handle);

	glDeleteTextures(0, &textureID);
}

void TextureManager::setupDefaultTextures() {
	//---------ALBEDO---------

	std::vector<unsigned char> data = {
		0xFF,
		0x00,
		0xFF,
	};

	TextureManager::TextureSpecification albedoSpecs = {
		.definition = { 
			.filtering = GL_NEAREST,
			.format = GL_RGB8,
            .width = 1,
            .height = 1,
		},

		.data = TextureManager::TextureData { .format = GL_RGB,
                       .encoding = GL_UNSIGNED_BYTE,
                       .data = data },
	};

	TextureManager::createTexture(albedoSpecs, TextureHandle::DEFAULT_ALBEDO);

	//---------NORMAL---------

	data = {
		0x7F,
		0x7F,
		0xFF,
	};

	TextureManager::TextureSpecification normalSpecs = {
		.definition = { 
			.filtering = GL_NEAREST,
			.format = GL_RGB8,
            .width = 1,
            .height = 1,
		},

		.data =
			TextureManager::TextureData {
										 .format = GL_RGB,
										 .encoding = GL_UNSIGNED_BYTE,
										 .data = data,
										 },
	};

	TextureManager::createTexture(normalSpecs, TextureHandle::DEFAULT_NORMAL);

	//---------ROUGHNESS---------

	data = {
		0x00,
		0x7F,
		0x00,
	};

	TextureManager::TextureSpecification metallicRoughnessSpecs = {
		.definition = { 
			.filtering = GL_NEAREST,
			.format = GL_RGB8,
            .width = 1,
            .height = 1,
			
		},

		.data =
			TextureManager::TextureData {
										 .format = GL_RGB,
										 .encoding = GL_UNSIGNED_BYTE,
										 .data = data,
										 },
	};

	TextureManager::createTexture(
		metallicRoughnessSpecs, TextureHandle::DEFAULT_ROUGHNESS_METALLIC
	);

	//---------EMISSION---------

	data = {
		0x00,
	};

	TextureManager::TextureSpecification emissionSpecs = {
		.definition = { 
			.filtering = GL_NEAREST,
			.format = GL_RGB8,
            .width = 1,
            .height = 1,
		},

		.data =
			TextureManager::TextureData {
										 .format = GL_RED,
										 .encoding = GL_UNSIGNED_BYTE,
										 .data = data,
										 },
	};

	TextureManager::createTexture(
		emissionSpecs, TextureHandle::DEFAULT_EMISSION
	);
}

void channelsToFormat(
	int channels, GLenum& dataFormat, GLenum& internalFormat
) {
	switch (channels) {
		case 1:
			dataFormat = GL_RED;
			internalFormat = GL_R8;
			break;
		case 2:
			dataFormat = GL_RG;
			internalFormat = GL_RG8;
			break;

		case 3:
			dataFormat = GL_RGB;
			internalFormat = GL_RGB8;
			break;

		case 4:
			dataFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
			break;

		default:
			dataFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
	}
}

std::vector<unsigned char> loadData(
	std::filesystem::path path, int& width, int& height, int& channels
) {
	std::vector<unsigned char> data;

	unsigned char* rawData = nullptr;
	bool isHDR = path.extension() == ".hdr";
	if (isHDR)
		rawData = (unsigned char*)stbi_loadf(
			path.string().data(), &width, &height, &channels, 0
		);
	else
		rawData =
			stbi_load(path.string().data(), &width, &height, &channels, 0);

	if (rawData == nullptr && stbi_failure_reason()) {
		std::cout << "Error loading " + path.string() + "|"
				  << "Failed for error " << stbi_failure_reason() << std::endl;
		return data;
	}

	data = std::vector<unsigned char>(
		rawData, rawData + width * height * channels * (isHDR ? 4 : 1)
	);
	return data;
}

TextureHandle TextureManager::loadTexture(
	const std::filesystem::path& path, TextureHandle handle
) {
	assert(std::filesystem::exists(path));
	std::vector<unsigned char> data;
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	bool isCubemap = !std::filesystem::is_regular_file(path);
	bool isHDR = path.has_extension() && path.extension() == ".hdr";

	if (isCubemap) {
		for (int i = 0; i < 6; i++) {
			auto imageData =
				loadData(path / CUBEMAP_FACES[i], width, height, channels);
			data.insert(data.end(), imageData.begin(), imageData.end());
		}
	} else {
		data = loadData(path, width, height, channels);
	}
	GLenum dataFormat, internalFormat, encoding;
	channelsToFormat(channels, dataFormat, internalFormat);
	encoding = GL_UNSIGNED_BYTE;

	Texture definition = {
		.filtering = GL_LINEAR,
		.format = internalFormat,
		.mipmap_levels =
			(GLint)std::floor(std::log2(std::max(width, height))) + 1,
		.width = width,
		.height = height,
	};
	if (isCubemap) {
		definition.wrapping = GL_CLAMP_TO_EDGE;
		definition.type = GL_TEXTURE_CUBE_MAP;
	}

	if (isHDR) {
		encoding = (GLenum)GL_FLOAT;
		definition.format = GL_RGB16F;
	}

	TextureManager::TextureSpecification specs {
		.definition = definition,
		.data = TextureManager::TextureData { .format = dataFormat,
                                             .encoding = encoding,
                                             .data = data }
	};

	return createTexture(specs, handle);
}
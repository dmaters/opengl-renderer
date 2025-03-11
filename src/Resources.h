#pragma once
#include <cstdint>
template <typename T>
struct Handle {
	static const T UNASSIGNED;

	uint32_t value = 0;
	bool operator==(const Handle& other) const { return value == other.value; }
	bool operator<(const Handle& other) const { return value < other.value; }
};

struct MaterialHandle : public Handle<MaterialHandle> {
	static const MaterialHandle DEFAULT;
};
struct ProgramHandle : public Handle<ProgramHandle> {
	static const ProgramHandle FORWARD;
};
struct UBOHandle : public Handle<UBOHandle> {
	static const UBOHandle PROJECTION_VIEW;
};

struct TextureHandle : public Handle<TextureHandle> {
	static const TextureHandle DEFAULT_ALBEDO;
	static const TextureHandle DEFAULT_NORMAL;
	static const TextureHandle DEFAULT_ROUGHNESS_METALLIC;
	static const TextureHandle DEFAULT_EMISSION;
};

template <typename T>
const T Handle<T>::UNASSIGNED = { 0 };
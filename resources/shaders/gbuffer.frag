#version 460 core
#extension GL_ARB_bindless_texture : require

in vec3 Normal;
in vec2 TexCoords;
in vec3 WorldPos;

out vec4 outAlbedo;
out vec4 outNormal;
out vec4 outPosition;
out vec4 outRoughnessMetallic;

layout(bindless_sampler) uniform sampler2D albedo;
uniform vec4 albedo_color;
layout(bindless_sampler) uniform sampler2D normal;
layout(bindless_sampler) uniform sampler2D roughness_metallic;
uniform float roughness_value;
uniform float metallic_value;

uniform int components;

vec4 getAlbedo(vec2 uv) {
	if ((components & 1 << 0) != 0) return albedo_color;
	return texture(albedo, uv);
}
float getMetallic(vec2 uv) {
	if ((components & 1 << 1) != 0) return metallic_value;
	return texture(roughness_metallic, uv).b;
}
float getRoughness(vec2 uv) {
	if ((components & 1 << 2) != 0) return roughness_value;
	return texture(roughness_metallic, uv).g;
}

void main() {
	outAlbedo = getAlbedo(TexCoords);

	vec3 dp1 = dFdx(WorldPos);
	vec3 dp2 = dFdy(WorldPos);
	vec2 duv1 = dFdx(TexCoords);
	vec2 duv2 = dFdy(TexCoords);

	float f = 1.0 / (duv1.x * duv2.y - duv2.x * duv1.y);

	vec3 tangent = normalize(f * (duv2.y * dp1 - duv1.y * dp2));
	vec3 bitangent = normalize(f * (-duv2.x * dp1 + duv1.x * dp2));

	mat3 TBN = mat3(tangent, bitangent, Normal);
	vec3 localNormal = normalize(texture(normal, TexCoords).rgb * 2.0 - 1.0);
	vec3 transformedNormal = TBN * localNormal;

	outNormal = vec4(transformedNormal, 1);
	outRoughnessMetallic =
		vec4(getRoughness(TexCoords), getMetallic(TexCoords), 0, 1);
	outPosition = vec4(WorldPos, 1);
}
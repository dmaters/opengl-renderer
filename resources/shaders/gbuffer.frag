#version 460 core
#extension GL_ARB_bindless_texture : require

in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec2 TexCoords;
in vec3 WorldPos;

out vec4 outAlbedo;
out vec4 outNormal;
out vec4 outPosition;
out vec4 outMetallicRoughness;

struct MaterialInstance {
	uint albedo;
	uint normal;
	uint metallic_roughness;
	uint components;
	vec4 albedo_color;
	float roughness_value;
	float metallic_value;
};

layout(binding = 1) uniform _textures { sampler2D textures[128]; };

layout(std140, binding = 2) uniform material_instances {
	MaterialInstance instances[128];
};

uniform int instance_index;

vec4 getAlbedo(vec2 uv) {
	if ((instances[instance_index].components & 1 << 0) != 0)
		return instances[instance_index].albedo_color;
	return texture(textures[instances[instance_index].albedo], uv);
}
float getMetallic(vec2 uv) {
	if ((instances[instance_index].components & 1 << 1) != 0)
		return instances[instance_index].metallic_value;
	return texture(textures[instances[instance_index].metallic_roughness], uv)
	    .b;
}
float getRoughness(vec2 uv) {
	if ((instances[instance_index].components & 1 << 2) != 0)
		return instances[instance_index].roughness_value;
	return texture(textures[instances[instance_index].metallic_roughness], uv)
	    .g;
}

void main() {
	vec4 albedo = getAlbedo(TexCoords);
	if (albedo.w < .5) discard;

	outAlbedo = albedo;

	mat3 TBN = mat3(Tangent, BiTangent, Normal);
	vec3 localNormal = normalize(
		texture(textures[instances[instance_index].normal], TexCoords).rgb *
			2.0 -
		1.0
	);
	vec3 transformedNormal = TBN * localNormal;

	outNormal = vec4(normalize(transformedNormal), 1);
	outMetallicRoughness =
		vec4(getMetallic(TexCoords), getRoughness(TexCoords), 0, 1);
	outPosition = vec4(WorldPos, 1);
}
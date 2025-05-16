#version 460 core
#extension GL_ARB_bindless_texture : require

in vec3 Normal;
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
	    .g;
}
float getRoughness(vec2 uv) {
	if ((instances[instance_index].components & 1 << 2) != 0)
		return instances[instance_index].roughness_value;
	return texture(textures[instances[instance_index].metallic_roughness], uv)
	    .b;
}

void main() {
	vec4 albedo = getAlbedo(TexCoords);
	if (albedo.w < .5) discard;

	outAlbedo = albedo;

	vec3 dp1 = dFdx(WorldPos);
	vec3 dp2 = dFdy(WorldPos);
	vec2 duv1 = dFdx(TexCoords);
	vec2 duv2 = dFdy(TexCoords);

	float f = 1.0 / (duv1.x * duv2.y - duv2.x * duv1.y);

	vec3 tangent = normalize(f * (duv2.y * dp1 - duv1.y * dp2));
	vec3 bitangent = normalize(f * (-duv2.x * dp1 + duv1.x * dp2));

	mat3 TBN = mat3(tangent, bitangent, Normal);
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
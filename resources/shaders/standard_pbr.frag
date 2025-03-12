#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPos;

layout(bindless_sampler) uniform sampler2D albedo;
uniform vec4 albedo_color;
layout(bindless_sampler) uniform sampler2D normal;
layout(bindless_sampler) uniform sampler2D roughness_metallic;
uniform float roughness_value;
uniform float metallic_value;
layout(bindless_sampler) uniform sampler2D emissive;
uniform float emissive_value;

uniform int components;

layout(std140, binding = 0) uniform projection_view {
	mat4 view;
	mat4 projection;
};

#define PI 3.1415926535897932384626433832795

struct Light {
	vec4 color;
	mat4 light_tranformation;
	mat4 light_projection;
	uint64_t shadow_map;
};

layout(std140, binding = 3) uniform LightsData {
	uint count;
	uint scene_light;
	Light[2] lights;
};

layout(location = 3) uniform samplerCube skybox;

float shadowPercentageOmni(int light) {
	vec4 position = lights[light].light_tranformation * FragPos;

	return texture(samplerCubeShadow(lights[light].shadow_map), position);
}

float shadowPercentage(int light) {
	vec4 fragPosLightSpace = lights[light].light_projection *
	                         lights[light].light_tranformation * FragPos;

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;

	float depth = texture(sampler2D(lights[light].shadow_map), projCoords.xy).r;
	float currentDepth = projCoords.z;

	return (currentDepth - 0.0005) > depth ? 1.0 : 0;
}

bool is_omni_shadow(int light) {
	return lights[light].light_projection[3][3] == 0;
}

float distributionGGX(float NdotH, float a) {
	float a2 = a * a;

	float f = (NdotH * a2 - NdotH) * NdotH + 1.0;
	return a2 / (PI * f * f);
}

// Schlick Fresnel approximation
vec3 fresnelSchlick(float HdotV, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}

// Smith Geometry Function for GGX
float geometrySmith(float NdotV, float NdotL, float a) {
	float a2 = a * a;
	float GGXV = NdotL * sqrt((NdotV - NdotV * a2) * NdotV + a2);
	float GGXL = NdotV * sqrt((NdotL - NdotL * a2) * NdotL + a2);
	return 0.5 / (GGXV + GGXL);
}
vec4 getAlbedo(vec2 uv) {
	if ((components & 1 << 0) != 0) return albedo_color;
	return texture(albedo, uv);
}
float getMetallic(vec2 uv) {
	if ((components & 1 << 1) != 0) return metallic_value;
	return texture(roughness_metallic, uv).y;
}
float getRoughness(vec2 uv) {
	if ((components & 1 << 2) != 0) return roughness_value;
	return texture(roughness_metallic, uv).x;
}
float getEmissive(vec2 uv) {
	if ((components & 1 << 3) != 0) return emissive_value;
	return texture(emissive, uv).r;
}

vec3 microfacetBRDF(vec2 uv, vec3 viewDir, vec3 lightDir) {
	vec3 halfview = normalize(viewDir + lightDir);
	vec3 _norm = Normal;
	float roughness = getRoughness(uv);
	float metallic = getMetallic(uv);
	vec3 albedo = getAlbedo(uv).rgb;
	// Dot products
	float NdotV = max(dot(_norm, viewDir), 0.0);
	float NdotL = max(dot(_norm, lightDir), 0.0);
	float NdotH = max(dot(_norm, halfview), 0.0);
	float HdotV = max(dot(halfview, viewDir), 0.0);

	// Base reflectivity
	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	float a = max(roughness * roughness, 0.002025);

	// BRDF components
	float NDF = distributionGGX(NdotH, a);
	vec3 F = fresnelSchlick(HdotV, F0);
	float V = geometrySmith(NdotV, NdotL, a);

	vec3 specular = NDF * F * V;

	vec3 diffuseColor = (1.0 - metallic) * albedo;
	vec3 diffuse = diffuseColor / PI;

	return (diffuse + specular) * NdotL;
}

void main() {
	vec3 view_dir = normalize(-FragPos.xyz);

	vec3 brdf = vec3(0);

	for (int i = 0; i < count; i++) {
		float shadow;
		vec3 light_dir;

		if (is_omni_shadow(i)) {
			shadow = 1 - shadowPercentageOmni(i);
			light_dir = lights[i].light_tranformation[3].xyz - FragPos.xyz;

		} else {
			shadow = 1 - shadowPercentage(i);
			light_dir = vec3(lights[i].light_tranformation * vec4(0, 0, -1, 0));
		}
		vec3 irradiance = microfacetBRDF(TexCoords, view_dir, light_dir);
		brdf += shadow * irradiance * lights[i].color.xyz * lights[i].color.w /
		        (pow(length(light_dir), 2) * 4 * PI);
	}
	FragColor = vec4(brdf, 1.);

	// FragColor = vec4(color.xyz, 1);
	//  ragColor = texture(albedo, TexCoords);
}
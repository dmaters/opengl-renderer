#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

layout(bindless_sampler) uniform sampler2D _albedo;
layout(bindless_sampler) uniform sampler2D _normal;
layout(bindless_sampler) uniform sampler2D _world_position;
layout(bindless_sampler) uniform sampler2D _metallic_roughness;

#define PI 3.1415926535897932384626433832795

layout(std140, binding = 0) uniform projection_view {
	mat4 view;
	mat4 projection;
};

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

float nonLinearToLinearDepth(float nonLinearDepth, mat4 projectionMatrix) {
	float a = projectionMatrix[2][2];
	float b = projectionMatrix[3][2];

	float near = -b / (a - 1.0);
	float far = -b / (a + 1.0);

	return (2.0 * near * far) / (far + near - nonLinearDepth * (far - near));
}
float shadowPercentageOmni(int light) {
	vec4 fragPos = texture(_world_position, TexCoord);

	mat4 lightTransform = lights[light].light_tranformation;
	vec4 lightSpacePos = lightTransform * fragPos;

	float depth =
		texture(
			samplerCube(lights[light].shadow_map), normalize(lightSpacePos.xyz)
		)
			.r;
	float linearDepth =
		nonLinearToLinearDepth(depth, lights[light].light_projection);
	float currentDepth = length(lightSpacePos.xyz);

	return (currentDepth - 0.0005) > -linearDepth ? 1.0 : 0;
}

float shadowPercentage(int light) {
	vec4 fragPos = texture(_world_position, TexCoord);

	vec4 fragPosLightSpace = lights[light].light_projection *
	                         lights[light].light_tranformation * fragPos;

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float depth = texture(sampler2D(lights[light].shadow_map), projCoords.xy).r;
	float currentDepth = projCoords.z;

	return currentDepth > depth + 0.005 ? 1.0 : 0;
}

bool is_omni_shadow(int light) {
	return lights[light].light_projection[3][3] == 0;
}

float distributionGGX(float NdotH, float a) {
	float a2 = a * a;

	float f = (NdotH * a2 - NdotH) * NdotH + 1.0;
	return a2 / (PI * f * f);
}

vec3 fresnelSchlick(float HdotV, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}

float geometrySchlickGGX(float NdotV, float a) {
	float r = (a + 1.0);
	float k = (r * r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(float NdotV, float NdotL, float a) {
	return geometrySchlickGGX(NdotV, a) * geometrySchlickGGX(NdotL, a);
}

vec3 microfacetBRDF(vec2 uv, vec3 viewDir, vec3 lightDir) {
	vec3 halfview = normalize(viewDir + lightDir);
	float roughness = max(texture(_metallic_roughness, uv).g, 0.05);
	float metallic = texture(_metallic_roughness, uv).r;
	vec3 albedo = texture(_albedo, uv).rgb;
	vec3 norm = texture(_normal, uv).rgb;

	float NdotV = max(dot(norm, viewDir), 0.0);
	float NdotL = max(dot(norm, lightDir), 0.0);
	float NdotH = max(dot(norm, halfview), 0.0);
	float HdotV = max(dot(halfview, viewDir), 0.0);

	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	float a = roughness * roughness;

	float NDF = distributionGGX(NdotH, a);
	vec3 F = fresnelSchlick(HdotV, F0);
	float V = geometrySmith(NdotV, NdotL, a);

	vec3 specular = NDF * F * V;

	vec3 diffuse = (1.0 - F) * (1.0 - metallic) * albedo / PI;

	return (diffuse + specular) * NdotL;
}

void main() {
	vec3 cameraPos = inverse(view)[3].xyz;
	vec4 fragPos = texture(_world_position, TexCoord);

	vec3 view_dir = normalize(cameraPos - fragPos.xyz);

	vec3 brdf = vec3(0);

	for (int i = 0; i < count; i++) {
		float shadow;
		mat4 light_mat = inverse(lights[i].light_tranformation);
		vec3 light_dir;
		if (is_omni_shadow(i)) {
			shadow = 1 - shadowPercentageOmni(i);
			light_dir = light_mat[3].xyz - fragPos.xyz;

		} else {
			shadow = 1 - shadowPercentage(i);
			light_dir = light_mat[2].xyz;
		}
		vec3 irradiance = microfacetBRDF(TexCoord, view_dir, light_dir);
		brdf += shadow * irradiance * lights[i].color.xyz * lights[i].color.w /
		        (pow(length(light_dir), 2) * 4 * PI);
	}

	FragColor = vec4(brdf, 1);
}
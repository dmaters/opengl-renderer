#version 460 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

layout(bindless_sampler) uniform sampler2D _albedo;
layout(bindless_sampler) uniform sampler2D _normal;
layout(bindless_sampler) uniform sampler2D _world_position;
layout(bindless_sampler) uniform sampler2D _metallic_roughness;

layout(bindless_sampler) uniform samplerCube irradiance_specular;
layout(bindless_sampler) uniform sampler2D brdf_lut;
layout(bindless_sampler) uniform samplerCube irradiance_diffuse;

layout(std140, binding = 0) uniform projection_view {
	mat4 view;
	mat4 projection;
};

#define PI 3.1415926535897932384626433832795

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) *
	                pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
	mat3 rotationMat = mat3(view);
	vec3 translation = vec3(view[3]);
	vec3 cameraPos = -transpose(rotationMat) * translation;
	vec4 fragPos = texture(_world_position, TexCoord);

	float metallic = texture(_metallic_roughness, TexCoord).r;
	vec3 albedo = texture(_albedo, TexCoord).rgb;
	float roughness = texture(_metallic_roughness, TexCoord).g;

	vec3 V = normalize(cameraPos - fragPos.xyz);
	vec3 N = texture(_normal, TexCoord).rgb;
	vec3 R = reflect(-V, N);

	vec3 F0 = mix(vec3(0.04), albedo, metallic);
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	vec3 irradiance = texture(irradiance_diffuse, N).rgb;
	vec3 diffuse = irradiance * albedo;
	vec3 prefilteredColor =
		textureLod(irradiance_specular, R, roughness * 5).rgb;
	vec2 envBRDF = texture(brdf_lut, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	vec3 ambient = (kD * diffuse + specular);

	FragColor = vec4(ambient, 1);
}
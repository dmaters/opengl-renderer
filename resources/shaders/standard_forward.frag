#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPos;


layout(location = 0) uniform sampler2D u_Albedo;
layout(location = 1) uniform sampler2D u_Normal;

uniform vec4 u_EnvironmentLighting;

layout(location = 2) uniform sampler2DShadow u_ShadowMap;


struct Light {
	vec3 color;
	vec3 position;
	vec3 direction;
	float intensity;

};
layout(std140) uniform u_LightsUBO {
	Light[512] lights;
	uint count;
};

layout(std140, binding = 1) uniform u_LightSpaceUBO {
    mat4 u_LightSpace;
};
layout(location = 3) uniform samplerCube u_Skybox;



float ambientShadows() {
	// Perform perspective divide
	vec4 fragPosLightSpace = u_LightSpace * FragPos;

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;


	projCoords = projCoords * 0.5 + 0.5;  // Transform to [0,1] range

	float shadow =
		texture(u_ShadowMap, projCoords);
	
	return floor(shadow);
}

void main() {


	vec3 norm = normalize(Normal);

	vec3 illumination = vec3(0);
	vec4 albedo = texture(u_Albedo, TexCoords);
	vec4 normal = texture(u_Normal, TexCoords);
	
	float shadow = ambientShadows();


	illumination +=
		albedo.rgb * (max(dot(norm, lights[0].direction.xyz), 0.0) *
						1 * (1 - shadow));





	vec4 color =
		vec4(illumination, 1) + vec4(0.3,.3,.3,1);

	FragColor = vec4(color.xyz, 1);
	// ragColor = texture(albedo, TexCoords);
}
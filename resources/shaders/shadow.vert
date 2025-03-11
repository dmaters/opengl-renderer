#version 460 core

#extension GL_ARB_bindless_texture : require

layout(location = 0) in vec3 aPos;
struct Light {
	vec4 color;
	mat4 light_tranformation;
	mat4 light_projection;
	sampler2D shadow_map;
};

layout(std140, binding = 3) uniform LightsData {
	uint count;
	uint scene_light;
	Light[2] lights;
};

uniform int current_light;
uniform mat4 model;

out vec4 FragPos;
void main() {
	gl_Position = lights[current_light].light_projection *
	              lights[current_light].light_tranformation * model *
	              vec4(aPos, 1.0);

	FragPos = gl_Position;
}
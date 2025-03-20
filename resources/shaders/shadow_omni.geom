#version 460 core

#extension GL_ARB_bindless_texture : require

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(binding = 2) uniform shadow_matrices { mat4 rotations[6]; };

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

void main() {
	for (int face = 0; face < 6; face++) {
		gl_Layer = face;
		for (int i = 0; i < 3; i++) {
			gl_Position = lights[current_light].light_projection *
			              lights[current_light].light_tranformation *
			              rotations[face] * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}
#version 460 core

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_TextCoords;

out vec3 Normal;
out vec2 TexCoords;
out vec3 WorldPos;

layout(std140, binding = 0) uniform projection_view {
	mat4 view;
	mat4 projection;
};
uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(i_Position, 1);

	Normal = i_Normal;
	TexCoords = i_TextCoords;
	WorldPos = (model * vec4(i_Position, 1)).xyz;
}
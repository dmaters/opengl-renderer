#version 430 core

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_TextCoords;

out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPos;

layout(std140) uniform u_projectionView {
	mat4 u_view;
	mat4 u_projection;
};
uniform mat4 u_model;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(i_Position, 1);

	Normal = i_Normal;
	TexCoords = i_TextCoords;
	FragPos = u_model * vec4(i_Position, 1.0);
}
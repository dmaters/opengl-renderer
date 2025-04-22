
#version 460 core

out vec3 TexCoords;

layout(std140, binding = 0) uniform projection_view {
	mat4 view;
	mat4 projection;
};

void main() {
	vec2 uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position = vec4(uv * 2.0f + -1.0f, 1.0, 1.0f);

	TexCoords = vec3(
		inverse(projection * mat4(mat3(view))) * vec4(gl_Position.xy, 1, 1)
	);
}
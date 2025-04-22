#version 460 core

out vec4 FragColor;

uniform float exposure = 1.0;
uniform float gamma = 2.2;
uniform sampler2D attachment;
in vec2 TexCoord;

void main() {
	vec4 color = texture(attachment, TexCoord);

	vec3 mapped = vec3(1.0) - exp(-color.rgb * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));
	FragColor = vec4(mapped, color.w);
}
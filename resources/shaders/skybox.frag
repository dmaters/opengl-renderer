
#version 460 core
#define PI 3.1415926535897932384626433832795

out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D skybox;
vec2 sampleEquirectangular(vec3 dir) {
	dir = normalize(dir);
	float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
	float v = acos(clamp(-dir.y, -1.0, 1.0)) / PI;
	return vec2(u, v);
}
void main() { FragColor = texture(skybox, sampleEquirectangular(TexCoords)); }
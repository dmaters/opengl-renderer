#version 460 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoord;
out vec4 Color;
layout(bindless_sampler) uniform sampler2D _texture;

void main() { Color = texture(_texture, TexCoord); }

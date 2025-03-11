#version 430 core

out vec2 TexCoord; // texcoords are in the normalized [0,1] range for the viewport-filling quad part of the triangle
void main() {
        vec2 vertices[3] = vec2[3](
                vec2(-1.0, -1.0),
                vec2( 3.0, -1.0),
                vec2(-1.0,  3.0)
        );

    vec2 pos = vertices[gl_VertexID];
    TexCoord = pos * 0.5 + 0.5; // map to [0,1] range
    gl_Position = vec4(pos, 0.0, 1.0);
}
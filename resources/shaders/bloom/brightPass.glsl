#version 430 core


layout(local_size_x = 1,local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D u_input;
layout(rgba32f, binding = 1) uniform image2D u_Output;

uniform float u_threshold;

void main(){

    vec4 color = imageLoad(u_input,ivec2(gl_GlobalInvocationID.xy));
    float luma = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    float bloom = smoothstep(0.4, 0.6, luma);
    
    imageStore(u_Output, ivec2(gl_GlobalInvocationID.xy), vec4(color.xyz * bloom,bloom));
}
#version 430 core


layout(local_size_x = 1,local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D u_input1;
layout(rgba32f, binding = 1) uniform image2D u_input2;
layout(rgba32f, binding = 2) uniform image2D u_Output;

void main() {
    vec4 color1 = imageLoad(u_input1,ivec2(gl_GlobalInvocationID.xy));
    vec4 color2 = imageLoad(u_input2,ivec2(gl_GlobalInvocationID.xy));
    imageStore(u_Output,ivec2(gl_GlobalInvocationID.xy),color1 + color2);
}
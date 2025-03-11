#version 430 core


layout(local_size_x = 1,local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D u_input;
layout(rgba32f, binding = 1) uniform image2D u_Output;

void main() {

    vec4 average = vec4(0);
    average += imageLoad(u_input,ivec2(gl_GlobalInvocationID.xy) * 2);
    average += imageLoad(u_input,ivec2(gl_GlobalInvocationID.xy) * 2 + ivec2(1,0)) ;  
    average += imageLoad(u_input,ivec2(gl_GlobalInvocationID.xy) * 2 + ivec2(1,1));  
    average += imageLoad(u_input,ivec2(gl_GlobalInvocationID.xy) * 2 + ivec2(0,1)) ;  

    imageStore(u_Output,ivec2(gl_GlobalInvocationID.xy),average / 4);
    //imageStore(u_Output,ivec2(gl_GlobalInvocationID.xy),vec4(1));

}
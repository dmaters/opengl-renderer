
#version 430 core
#define KERNEL_SIZE 4

layout(local_size_x = KERNEL_SIZE,local_size_y = KERNEL_SIZE, local_size_z = 1) in;

layout(binding = 0) uniform sampler2D u_input;
layout(rgba32f, binding = 1) uniform image2D u_Output;

uniform int u_blurSize;
uniform int u_mipmapLevel;


shared vec4 samples[4][4];

void main() {


  
    ivec2 samplePosition = ivec2(gl_LocalInvocationID.xy) - ivec2(4,4);

    int sampleMultiplier =  u_blurSize / KERNEL_SIZE;

    samplePosition *= sampleMultiplier;

    ivec2 globalPosition = ivec2(gl_WorkGroupID.xy);


    vec4 color = vec4(0);
    for(int dx = 0; dx < sampleMultiplier; dx++){
        for(int dy = 0; dy < sampleMultiplier; dy++){
            ivec2 deltaPosition = samplePosition + ivec2(dx,dy) * ivec2( sign(samplePosition.x),sign(samplePosition.y));
            ivec2 localPosition = globalPosition + deltaPosition;
            
            if(localPosition.x < 0 || localPosition.x > gl_NumWorkGroups.x)
                localPosition.x = globalPosition.x;
            if(localPosition.y < 0 || localPosition.y > gl_NumWorkGroups.y)
                localPosition.y = globalPosition.y;
            

            
            color += textureLod(u_input,  vec2(localPosition.xy) / vec2(gl_NumWorkGroups.xy),float(u_mipmapLevel));
          



        }   
    }

    samples[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = color / float(sampleMultiplier);

    barrier();

    if(gl_LocalInvocationID.xy != uvec2(0,0))
        return;
        
    vec4 average;
    for (int x = 0; x <KERNEL_SIZE; x++) {
        for (int y = 0; y <KERNEL_SIZE; y++) {
            average += samples[x][y];
        }
    }

    imageStore(u_Output,ivec2(gl_WorkGroupID.xy),average  / (KERNEL_SIZE * KERNEL_SIZE));
   
}
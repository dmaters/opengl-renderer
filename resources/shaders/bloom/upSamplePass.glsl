#version 430 core


layout(local_size_x = 1,local_size_y = 1, local_size_z = 1) in;


layout(rgba32f, binding = 0) uniform image2D u_input;
layout(rgba32f, binding = 1) uniform image2D u_output;
layout(rgba32f, binding = 2) uniform image2D u_intermediate;

void main() {
    
    
    ivec2 texPos = ivec2(gl_GlobalInvocationID.xy);
    vec4 basePixel = imageLoad(u_input,texPos);
    vec4 intermediatePixel = imageLoad(u_intermediate,texPos * 2);
    imageStore(u_output,texPos * 2, (basePixel + intermediatePixel) / 2);

    vec4 x1 = (basePixel + imageLoad(u_input,texPos + ivec2(1,0))) / 2;
    intermediatePixel =  imageLoad(u_intermediate,texPos * 2 + ivec2(1,0));
    imageStore(u_output,texPos * 2 + ivec2(1,0), (x1 + intermediatePixel) / 2);
    
    
    vec4 xy = (basePixel + imageLoad(u_input,texPos + ivec2(1,1))) / 2;
    intermediatePixel =  imageLoad(u_intermediate,texPos * 2 + ivec2(1,1));
    imageStore(u_output,texPos * 2 + ivec2(1,1), (xy + intermediatePixel) / 2);
    
    vec4 y1 = (basePixel + imageLoad(u_input,texPos + ivec2(0,1))) / 2;
    intermediatePixel =  imageLoad(u_intermediate,texPos * 2 + ivec2(0,1));
    imageStore(u_output,texPos * 2 + ivec2(0,1), (y1 + intermediatePixel) / 2);
    

}
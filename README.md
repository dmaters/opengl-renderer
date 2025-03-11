# OpenGL GLTF Renderer

A simple OpenGL forward renderer for with shadow maps and basic PBR (Physically Based Rendering) support. This project is intended to be an introduction to the OpenGL API and computer graphics in general, so it won't have advanced features nor will it have future development, other than the intended features. 

This project is superseded by my (vulkan renderer)(https://github.com/dmaters/vulkan-renderer.git).


## Features
- Physically Based Rendering (PBR) with basic material support, trasparency (in development), and HDR
- Bindless Rendering (in development)
- Cubemapped Skybox
- Shadow mapping for directional and point lights
- Frustum culling
- Compute bloom post-processing (in development)


## Building the Project
1. Clone the repository:
```bash
   git clone https://github.com/dmaters/opengl-renderer.git
   cd opengl-gltf-renderer
```
2. Configure the project with CMake:
```bash
    mkdir build
    cd build
    cmake ..
```
3. Build the project:

```bash
    cmake --build .
```
4. Run the executable:

```bash
    ./renderer.exe {path to obj}
```

## Usage

Load a model by passing the location as an command line argument.
Lighting and camera position can be adjusted in the source code (src/Application.cpp).
The renderer supports basic PBR materials and directional and point light shadows.
Use the mouse and keyboard to navigate the scene.

## Limitations

Limited material support (only basic PBR properties are implemented).
No advanced rendering features like light culling, anti-aliasing, global illumination, etc. 
No scene managment, for model switching application has to be re-launched.

## Notable features

1. Skybox is rendered in screen-space
2. Point light shadow-mapping, using cubemapped depth buffers managed with geometry shaders.
3. Modern OpenGL with DSA/AZDO 
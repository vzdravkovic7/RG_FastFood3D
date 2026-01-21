# RG_FastFood_3D

3D OpenGL project in C++ simulating a fast-food ordering and burger-assembly scenario.  
This project is an extension of the original 2D version, upgraded with 3D geometry, perspective camera and basic interaction in 3 axes.

## Features

- Fullscreen display with 75 FPS frame limiter  
- Escape key exits at any time  
- Depth test and back-face culling toggles  
- Perspective camera (movement with arrow keys, mouse to look around)  
- Raw → cooked pattie state with progress bar  
- 3D stove (textured box), 3D table (box with legs)  
- 3D ketchup and mustard bottles (cylinder + cone)  
- 3D pattie (flattened cylinder)  
- Ingredients as textured 2D planar quads facing upward  
- 2D UI overlay: button, loading bar, “Prijatno!” text, signature  

## Project Setup

- C++17  
- OpenGL 3.3 Core Profile  
- GLFW + GLEW (NuGet)  
- stb_image for texture loading  
- Shaders stored in `shaders/`  
- Textures stored in `res/`  

## Build Instructions

1. Clone repo.
2. Open the `.sln` in Visual Studio 2022.
3. Restore NuGet packages (GLEW, GLFW).
4. Build in Release x64.
5. Run and interact using:
   - **WASD**: move objects (x/z axes)
   - **SPACE / SHIFT**: move object on +Y / -Y
   - **Arrow keys**: move camera
   - **Mouse move**: look around
   - **ESC**: exit program

## Project Structure

   - include/ - headers
   - src/ - cpp sources
   - res/ - textures
   - shaders/ - vertex & fragment shaders

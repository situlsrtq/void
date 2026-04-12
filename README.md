
# [void]: Maybe an Engine One Day

## Building from Source

Both Windows and Linux are supported by default. The project can be built from source using CMAKE and the provided
CMakeLists.txt file. Optional modules are controlled by CMake option toggles, it will yell at you if anything is misconfigured.

Nearly all vendor files required to build the software are already included in the repo. The single exception is 
OpenGL, which is included by default on standard Windows packages. Linux installations require a version of libGL 
that supports your hardware (either downloaded as a standalone package or, most likely, from the vendor utils your build uses) -
this library must be accessible from your PATH.

This software is written for OpenGL Core Profile 4.6.

## Vendor libraries:

[GLAD](https://github.com/dav1dde/glad): Managed OpenGL function pointers for cross-platform, cross-version compatibility\
[GLFW](https://github.com/glfw/glfw): Basic cross-platform windowing and input control\
[GLM](https://github.com/g-truc/glm): Math utilities\
[CGLTF](https://github.com/jkuhlmann/cgltf): Parsing support for GLTF 2.0 files\
[DearImGUI](https://github.com/ocornut/imgui): Immediate-mode UI framework for menus and controls\
[Tracy](https://github.com/wolfpld/tracy): My beloved, profiling

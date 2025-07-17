
# [void]: Real-Time 3D Render Tool

This project implements various 3D rendering algorithms to provide a basic sandbox for users to interact with. The lighting model uses a single, real time, Phong positional source (for now). 

Dynamic allocation from the OS occurs exactly once, at initialization - all runtime dynamic objects are handled 
by custom linear/bump allocators that supports free operations.

## Vendor libraries:

OpenGL: Cross-platform API for rendering 2D/3D graphics on GPU hardware\
GLAD: Provides managed OpenGL function pointers for cross-platform, cross-version compatibility\
GLFW: Provides basic cross-platform windowing and input control\
DearImGUI: Provides an immediate-mode UI framework for menus and controls 

With the exception of a few functions from C built-ins (string, math, stdint, stdio), everything else is hand-implemented. 
The standard library is not used. For the most part, the linear algebra in uMATH, uPHYS and the shaders are derived through research (among the various sources listed below), not by hand.

## Building from Source

Both Windows and Linux are supported by default. The project can be built from source using CMAKE and the provided
CMakeLists.txt file. 

Nearly all vendor files required to build the software are already included in the repo. The single exception is 
OpenGL, which comes as part of the standard Windows package. Linux installations require a version of libGL 
that supports your hardware (either downloaded as a standalone package or from the vendor utils your build uses) -
this library must be accessible from your PATH.

This software is written for OpenGL Core Profile 4.6, but will most likely work on any Core profile 3.0+. Changes can be
applied in main.cpp and the shader files, if desired.

## Sources
#### From which linear algebra operations and 3D algorithms were primarily researched prior to implementation

LearnOpenGL | https://learnopengl.com/ \
Linear Algebra Done Right | https://linear.axler.net/ \
OGL Dev | https://www.ogldev.org/index.html \
OpenGL 4.6 (Core Profile) Specification | https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf \
Real Time Rendering, Fourth Edition | https://www.realtimerendering.com/ \
Song Ho's Notes | http://www.songho.ca/index.html \
Wikipedia | https://www.wikipedia.org/

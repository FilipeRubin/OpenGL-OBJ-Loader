# OpenGL-OBJ-Loader
Just a simple class that loads obj files into OpenGL vertex buffers.

# How to install
* Just drag the files [OBJ.cpp](OBJ/OBJ.cpp) and [OBJ.hpp](OBJ/OBJ.hpp) into your OpenGL C++ project.

# How to use
* After `#include "OBJ/OBJ.hpp"`, create an instance of type OBJ. You can choose between loading the model on the constructor or leaving it empty to load the model later with `OBJ::Load()`.
* In your main loop, use `OBJ::Render()` to render the loaded model.

# Notes
* This implementation supposes you're using GLAD as your OpenGL library. If you're using any other OpenGL library you can just replace the `#include <glad/glad.h>` inside [OBJ.hpp](OBJ/OBJ.hpp) and it should work normally.
* Simillarly, if your path to GLAD is different than mine you can just replace the `#include` of the above mentioned file.
* This is a very simple OBJ loader, it does not handle groups, objects or any _fancy_ stuff, it just loads vertices (including position, normal, and texture coordinates) and stores it into a buffer.
* This does not handle textures, materials or .mtl files.
* This does not include a shader, just make sure your vertex shader has, respectively, a position, normal, and UV coordinates.

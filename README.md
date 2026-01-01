# Opengl game

An opengl game made with a custom opengl engine built for fun in c.
The project is currently in the initial engine building process.

## Features
* Scene system
* Resource manager
* Materials with lighting
* Obj file loading

## Dependencies
* GLFW3
* glad
* OpenGL
* stb\_image

## Building

### Linux
```
make -f LinuxMakefile
```

### Windows
```
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```
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

The project is built using cmake and works for both linux and windows.
The building is not tested on mac but might work.

### Linux
```
git clone https://github.com/Samisalami05/opengl-game.git
cd opengl-game
cmake -B build
cmake --build build
```

### Windows
```
git clone https://github.com/Samisalami05/opengl-game.git
cd opengl-game
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

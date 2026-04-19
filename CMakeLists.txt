cmake_minimum_required(VERSION 3.23)
project(opengl-game LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

# Disable GLFW extras
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_custom_target(clean-engine
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR}/CMakeFiles/engine.dir
        $<TARGET_FILE:engine>
)

# GLFW
add_subdirectory(libs/glfw-3.4)

# GLAD
add_library(glad STATIC libs/glad/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

# ASSIMP
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
include_directories(/usr/include/minizip)
add_subdirectory(libs/assimp)

# CIMGUI
add_subdirectory(libs/cimgui)

file(GLOB_RECURSE ENGINE_SRC
    src/engine/*.c
)

file(GLOB_RECURSE EDITOR_SRC
    src/editor/*.c
)

file(GLOB_RECURSE TEST_SRC
    tests/*.c
)

add_library(engine STATIC ${ENGINE_SRC})
add_library(editor STATIC ${EDITOR_SRC})

target_include_directories(engine PUBLIC
    src/engine
    src
    libs
    libs/glfw-3.4/include
)

target_include_directories(editor PUBLIC
    src/engine
    src
    libs
    libs/glfw-3.4/include
	libs/cimgui
	libs/cimgui/imgui
	libs/cimgui/imgui/backends
)

target_link_libraries(engine PUBLIC glad)
target_link_libraries(engine PRIVATE glfw cimgui assimp)

target_link_libraries(editor PUBLIC glad)
target_link_libraries(editor PRIVATE glfw cimgui assimp)

target_sources(editor PRIVATE
    libs/cimgui/imgui/backends/imgui_impl_glfw.cpp
    libs/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)

if (WIN32)
    target_link_libraries(engine PRIVATE opengl32)
	target_link_libraries(editor PRIVATE opengl32)
else()
	target_link_libraries(engine PRIVATE GL openal dl m)
	target_link_libraries(editor PRIVATE GL openal dl m)
endif()

add_executable(main src/main.c)
target_link_libraries(main PRIVATE engine editor)

add_executable(runtests ${TEST_SRC})
target_link_libraries(runtests PRIVATE engine)

# Test
add_test(NAME all_tests COMMAND runtests)

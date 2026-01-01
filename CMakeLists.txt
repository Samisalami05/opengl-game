cmake_minimum_required(VERSION 3.23)
project(opengl-game)

set(CMAKE_C_STANDARD 11)

enable_testing()

# Disable GLFW extras
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.4)

file(GLOB_RECURSE SRC
    src/*.c
    libs/glad/glad.c
)

add_executable(main ${SRC})

target_include_directories(main PRIVATE
    src
    libs
    libs/glad/include
)

target_link_libraries(main PRIVATE glfw)

if (WIN32)
    target_link_libraries(main PRIVATE opengl32)
else()
    target_link_libraries(main PRIVATE GL dl m)
endif()
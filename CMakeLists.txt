cmake_minimum_required(VERSION 3.23)
project(opengl-game)

set(CMAKE_C_STANDARD 11)

enable_testing()

# Disable GLFW extras
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLFW
add_subdirectory(libs/glfw-3.4)

# GLAD
add_library(glad STATIC libs/glad/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

file(GLOB_RECURSE APP_SRC
    src/engine/*.c
)

file(GLOB_RECURSE TEST_SRC
    tests/*.c
)

add_library(engine STATIC ${APP_SRC})

target_include_directories(engine PUBLIC
    src/engine
    src
    libs
    libs/glfw-3.4/include
)
target_link_libraries(engine PUBLIC glad)
target_link_libraries(engine PRIVATE glfw)

if (WIN32)
    target_link_libraries(engine PRIVATE opengl32)
else()
    target_link_libraries(engine PRIVATE GL dl m)
endif()

add_executable(main src/main.c)
target_link_libraries(main PRIVATE engine)

add_executable(runtests ${TEST_SRC})
target_link_libraries(runtests PRIVATE engine)

# Test
add_test(NAME all_tests COMMAND runtests)
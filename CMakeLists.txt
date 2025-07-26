cmake_minimum_required(VERSION 4.0)
project(GraphicsApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/glad.c
)

# Include GLAD and GLFW headers
include_directories(include)

# Add executable
add_executable(GraphicsApp ${SOURCES})

# Find and link OpenGL and GLFW
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

target_include_directories(GraphicsApp PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(GraphicsApp PRIVATE ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY})

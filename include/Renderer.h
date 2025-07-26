#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "ShaderProgram.h"

class Renderer {
private:
    GLFWwindow* window = nullptr;
    ShaderProgram* shaderProgram = nullptr;

    static void windowSizeCallback(GLFWwindow* window, int width, int height);

    bool initOpenGL(const char* name, uint32_t width, uint32_t height);

    void processInput();

public:
    Renderer(const char* name, uint32_t width = 800, uint32_t height = 600);

    bool active();

    void process();

    ~Renderer();
};
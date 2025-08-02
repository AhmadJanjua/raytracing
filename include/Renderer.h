#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "ShaderProgram.h"
#include "ComputeShader.h"
#include "Camera.h"

struct Time {
    float dt;
    float prev;
    int frames;

    void print() {
        float cur = glfwGetTime();
        dt = cur - prev;
        prev = cur;
        if (frames > 60) {
            std::cout << "FPS: " << 1 / dt << std::endl;
            frames = 0;
        } else {
            frames++;
        }
    }
};

class Renderer {
private:
    GLFWwindow* window = nullptr;
    ShaderProgram* shaderProgram = nullptr;
    ComputeShader* computeProgram = nullptr;
    Camera camera;
    double mousePosX, mousePosY;
    Time time;

    static void windowSizeCallback(GLFWwindow* window, int width, int height);

    bool initOpenGL(const char* name, uint32_t width, uint32_t height);
    void processInput();
public:
    Renderer(const char* name, uint32_t width = 800, uint32_t height = 600);
    ~Renderer();

    bool active();
    void process();
    
};
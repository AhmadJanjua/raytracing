#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "TextFile.h"
#include <iostream>

const float vertices[] = {
     0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
};

const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

class ShaderProgram {
private:
    uint32_t VBO, VAO, EBO;
    uint32_t shaderID;

    void shaderCompileLog(uint32_t ID, const char* err);

    void shaderLinkLog();

    void genShader(uint32_t& ID, uint32_t SHADER_TYPE, const char* src, const char* err);

    void configBufferLayout();
public:    
    ShaderProgram(const char* vertFilename, const char* fragFilename);

    ~ShaderProgram();

    void bind();

    void unbind();
};


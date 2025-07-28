#pragma once

#include <glad/glad.h>

#include "TextFile.h"
#include "ShaderHelper.h"

class ComputeShader {
private:
    uint32_t textID[2];
    uint32_t ID, height, width;
    bool frameIdx = 0;

public:
    ComputeShader(const char* filename, uint32_t width, uint32_t height);
    ~ComputeShader();

    void run(float t);
    void display(uint32_t progShaderID, uint32_t VAO);
    void swapBuffers();
};
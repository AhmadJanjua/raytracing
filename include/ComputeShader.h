#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TextFile.h"
#include "ShaderHelper.h"
#include "Camera.h"

class ComputeShader {
private:
    uint32_t textID[2];
    uint32_t ID, height, width;
    bool frameIdx;

public:
    ComputeShader(const char* filename, uint32_t width, uint32_t height);
    ~ComputeShader();

    void run(float t, Camera& c);
    void display(uint32_t progShaderID, uint32_t VAO);
    void swapBuffers();
    void updateSize(uint32_t width, uint32_t height);
};

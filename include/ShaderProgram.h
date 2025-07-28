#pragma once

#include <glad/glad.h>

#include "TextFile.h"
#include "ShaderHelper.h"

#include <iostream>

const float screenTriangle[] = {
    -1.0f, -1.0f,
     3.0f, -1.0f,
    -1.0f,  3.0f
};

class ShaderProgram {
private:
    uint32_t VBO, VAO;
    uint32_t ID;

    void configBufferLayout();
public:    
    ShaderProgram(const char* vertFilename, const char* fragFilename);
    ~ShaderProgram();

    void bind();

    uint32_t getID() const;
    uint32_t getVAO() const;
    uint32_t getVBO() const;
};


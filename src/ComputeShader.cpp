#include "ComputeShader.h"

ComputeShader::ComputeShader(const char* filename, uint32_t width, uint32_t height)
: width(width), height(height) {
    TextFile compShaderSrc(filename);
    uint32_t compShaderID;

    ShaderHelper::genShader(
        compShaderID,
        GL_COMPUTE_SHADER,
        compShaderSrc,
        "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n"
    );

    ID = glCreateProgram();
    glAttachShader(ID, compShaderID);
    glLinkProgram(ID);
    ShaderHelper::shaderLinkLog(ID);

    glDeleteShader(compShaderID);


    glGenTextures(2, textID);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, textID[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

ComputeShader::~ComputeShader() {
    glDeleteTextures(2, textID);
    glDeleteProgram(ID);
}

void ComputeShader::run(float t) {
    glUseProgram(ID);

    glBindImageTexture(0, textID[frameIdx], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, textID[!frameIdx], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glUniform1f(glGetUniformLocation(ID, "t"), t);

    glDispatchCompute(width / 10, height / 10, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShader::display(uint32_t progShaderID, uint32_t VAO) {
    glUseProgram(progShaderID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textID[frameIdx]);
    glUniform1i(glGetUniformLocation(progShaderID, "pixels"), 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ComputeShader::swapBuffers() {
    frameIdx = !frameIdx;
}

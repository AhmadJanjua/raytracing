#include "ShaderHelper.h"

void ShaderHelper::shaderCompileLog(uint32_t ID, const char* err) {
    int success;
    char infoLog[512];

    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ID, 512, NULL, infoLog);
        std::cout << err << infoLog << std::endl;
    }
}

void ShaderHelper::shaderLinkLog(uint32_t ID) {
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout
            << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << std::endl;
    }
}

void ShaderHelper::genShader(uint32_t& ID, uint32_t SHADER_TYPE, const char* src, const char* err) {
    ID = glCreateShader(SHADER_TYPE);
    glShaderSource(ID, 1, &src, NULL);
    glCompileShader(ID);

    shaderCompileLog(ID, err);
}
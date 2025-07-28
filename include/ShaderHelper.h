#pragma once

#include <glad/glad.h>

#include <cstdint>
#include <iostream>

namespace ShaderHelper {
    void shaderCompileLog(uint32_t ID, const char* err);
    void shaderLinkLog(uint32_t ID);
    void genShader(uint32_t& ID, uint32_t SHADER_TYPE, const char* src, const char* err);
};

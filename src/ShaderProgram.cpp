#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertFilename, const char* fragFilename) {
    TextFile vertShaderSrc(vertFilename);
    TextFile fragShaderSrc(fragFilename);

    uint32_t vertShaderID;
    uint32_t fragShaderID;

    ShaderHelper::genShader(
        vertShaderID,
        GL_VERTEX_SHADER,
        vertShaderSrc,
        "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
    );

    ShaderHelper::genShader(
        fragShaderID,
        GL_FRAGMENT_SHADER,
        fragShaderSrc,
        "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
    );

    // link shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertShaderID);
    glAttachShader(ID, fragShaderID);
    glLinkProgram(ID);
    ShaderHelper::shaderLinkLog(ID);

    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    configBufferLayout();
}

ShaderProgram::~ShaderProgram() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ID);
}

void ShaderProgram::configBufferLayout() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenTriangle), screenTriangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void ShaderProgram::bind() {
    glUseProgram(ID);
    glBindVertexArray(VAO);
}

uint32_t ShaderProgram::getID() const {
    return ID;
}

uint32_t ShaderProgram::getVAO() const {
    return VAO;
}

uint32_t ShaderProgram::getVBO() const {
    return VBO;
}

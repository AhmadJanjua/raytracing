#include "ShaderProgram.h"

void ShaderProgram::shaderCompileLog(uint32_t ID, const char* err) {
    int success;
    char infoLog[512];

    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ID, 512, NULL, infoLog);
        std::cout << err << infoLog << std::endl;
    }
}

void ShaderProgram::shaderLinkLog() {
    int success;
    char infoLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        std::cout
            << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << std::endl;
    }
}

void ShaderProgram::genShader(uint32_t& ID, uint32_t SHADER_TYPE, const char* src, const char* err) {
    ID = glCreateShader(SHADER_TYPE);
    glShaderSource(ID, 1, &src, NULL);
    glCompileShader(ID);

    shaderCompileLog(ID, err);
}

void ShaderProgram::configBufferLayout() {
    // Create Buffers
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

ShaderProgram::ShaderProgram(const char* vertFilename, const char* fragFilename) {
    TextFile vertShaderSrc(vertFilename);
    TextFile fragShaderSrc(fragFilename);

    uint32_t vertShaderID;
    uint32_t fragShaderID;

    genShader(
        vertShaderID,
        GL_VERTEX_SHADER,
        vertShaderSrc,
        "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
    );

    genShader(
        fragShaderID,
        GL_FRAGMENT_SHADER,
        fragShaderSrc,
        "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
    );

    // link shaders
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertShaderID);
    glAttachShader(shaderID, fragShaderID);
    glLinkProgram(shaderID);
    shaderLinkLog();

    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    configBufferLayout();
}

ShaderProgram::~ShaderProgram() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderID);
}

void ShaderProgram::bind() {
    glUseProgram(shaderID);
    glBindVertexArray(VAO);
}

void ShaderProgram::unbind() {
    glBindVertexArray(0);
    glUseProgram(0);
}


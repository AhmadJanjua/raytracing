#include "Renderer.h"

void Renderer::windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool Renderer::initOpenGL(const char* name, uint32_t width, uint32_t height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name, NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}

void Renderer::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

Renderer::Renderer(const char* name, uint32_t width, uint32_t height) {
    if (!initOpenGL(name, width, height)) {
        exit(-1);
    }

    shaderProgram = new ShaderProgram(
        "../assets/shaders/manual.vert",
        "../assets/shaders/manual.frag"
    );
}

bool Renderer::active() {
    return !glfwWindowShouldClose(window);
}

void Renderer::process() {
    processInput();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    shaderProgram->unbind();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Renderer::~Renderer() {
    delete shaderProgram;
    shaderProgram = nullptr;
    glfwTerminate();
}

#include "Renderer.h"

Renderer::Renderer(const char* name, uint32_t width, uint32_t height) {
    if (!initOpenGL(name, width, height)) {
        exit(-1);
    }

    shaderProgram = new ShaderProgram(
        "../assets/shaders/vert.glsl",
        "../assets/shaders/frag.glsl"
    );

    computeProgram = new ComputeShader(
        "../assets/shaders/comp.glsl",
        width,
        height
    );
}

Renderer::~Renderer() {
    delete shaderProgram;
    delete computeProgram;
    shaderProgram = nullptr;
    glfwTerminate();
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
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, windowSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}

void Renderer::windowSizeCallback(GLFWwindow* window, int width, int height) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    renderer->computeProgram->updateSize(width, height);
    glViewport(0, 0, width, height);
}

void Renderer::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

bool Renderer::active() {
    return !glfwWindowShouldClose(window);
}

void Renderer::process() {
    time.print();

    processInput();

    computeProgram->run(time.prev);
    computeProgram->display(shaderProgram->getID(), shaderProgram->getVAO());
    computeProgram->swapBuffers();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

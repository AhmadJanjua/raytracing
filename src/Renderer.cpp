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

    glfwGetCursorPos(window, &mousePosX, &mousePosY);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS
    ) {
        camera.pos += camera.front * time.dt * 3.f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
    ) {
        camera.pos -= camera.front * time.dt * 3.f;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS
    ) {
        camera.pos += camera.right * time.dt * 3.f;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
    ) {
        camera.pos -= camera.right * time.dt * 3.f;
    }

    double tmpX, tmpY;
    glfwGetCursorPos(window, &tmpX, &tmpY);

    camera.phi -= (mousePosX - tmpX) * time.dt * 8.f;
    camera.theta += (mousePosY - tmpY) * time.dt * 8.f;

    mousePosX = tmpX;
    mousePosY = tmpY;
}

bool Renderer::active() {
    return !glfwWindowShouldClose(window);
}

void Renderer::process() {
    time.print();
    camera.update();

    processInput();

    computeProgram->run(time.prev, camera);
    computeProgram->display(shaderProgram->getID(), shaderProgram->getVAO());
    computeProgram->swapBuffers();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

#pragma once
#include "glm/glm.hpp"

struct Camera {
    float theta;
    float phi;
    glm::vec3 pos;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 front;

    Camera();
    void update();
};

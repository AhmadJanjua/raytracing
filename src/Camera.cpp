#include "Camera.h"

Camera::Camera() {
    theta = 0.f;
    phi = 180.f;

    pos = glm::vec3(0);

    float radPhi = glm::radians(phi);
    float radTheta = glm::radians(theta);

    front = {
        cos(radTheta) * sin(radPhi),
        sin(radTheta),
        cos(radTheta) * cos(radPhi)
    };

    front = glm::normalize(front);
    up = glm::vec3(0.f, 1.f, 0.f);
    right = glm::normalize(glm::cross(up, front));
    up = glm::normalize(glm::cross(front, right));
}

void Camera::update() {
    theta = glm::clamp(theta, -89.f, 89.f);
    float radPhi = glm::radians(phi);
    float radTheta = glm::radians(theta);

    front = {
        cos(radTheta) * sin(radPhi),
        sin(radTheta),
        cos(radTheta) * cos(radPhi)
    };

    front = glm::normalize(front);
    up = glm::vec3(0.f, 1.f, 0.f);
    right = glm::normalize(glm::cross(up, front));
    up = glm::normalize(glm::cross(front, right));
}
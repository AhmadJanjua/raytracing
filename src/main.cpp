#include "Renderer.h"

int main() {
    Renderer renderer("RayTracer", 1920, 1080);

    while (renderer.active()) {
        renderer.process();
    }

    return 0;
}
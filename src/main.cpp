#include "Renderer.cpp"

int main() {
    Renderer renderer("Hello");

    while (renderer.active()) {
        renderer.process();
    }

    return 0;
}
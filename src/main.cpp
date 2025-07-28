#include "Renderer.h"

int main() {
    Renderer renderer("Hello", 800, 800);

    while (renderer.active()) {
        renderer.process();
    }

    return 0;
}

#include "renderer.hpp"

int main() {

    Renderer renderer;
    renderer.init(600,400);

    while (!glfwWindowShouldClose(renderer.getWindow())) {
        glfwPollEvents();
        renderer.processInput();
        renderer.render();
    }

    return 0;
}
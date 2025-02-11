#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <chrono>
#include <thread>
#include <map>
#include "shader.hpp"


static void glfw_error_callback(int error, const char* description);

// The callbacks are updated and called BEFORE the Update loop is entered
// It can be assumed that inside the Update loop all callbacks have already been processed
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
static void key_callback(GLFWwindow* window, int key, int scancode, int actions, int mods);


class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(int width, int height);
    void renderFrame(const uint8_t* frameData, int width, int height,  double frameDelay=0.0f);
    void render();
    void cleanup();
    GLFWwindow* getWindow() const { return m_window; }
    void processInput() const;

private:
    GLFWwindow* m_window;
    GLuint m_texture;
    GLuint VAO, VBO, EBO;
    Shader* m_shader;

    void setupQuad();

};

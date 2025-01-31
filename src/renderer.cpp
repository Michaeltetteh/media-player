#include "renderer.hpp"
#include <iostream>

const char* GLSL_VERSION;

Renderer::Renderer() : m_window(nullptr), m_texture(0), m_shaderProgram(0), VAO(0), VBO(0), EBO(0) {}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::init(int width, int height) {
    std::cout<<"width: "<<width<< "height: "<<height<<"\n";
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return false;
    }
    glfwSetErrorCallback(glfw_error_callback);
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    GLSL_VERSION = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    printf("GL VERSION 3.2\n");
    GLSL_VERSION = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    GLSL_VERSION = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create a m_windowed mode m_window and its OpenGL context
    m_window = glfwCreateWindow(width, height, "Media Player", nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW m_window." << std::endl;
        glfwTerminate();
        return false;
    }

    // Make the m_window's context current
    glfwMakeContextCurrent(m_window);
    // glfwSwapInterval(1);  // Enable V-Sync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        return false;
    }

    // Set up the viewport
    glViewport(0, 0, width, height);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Initialize ImGui for GLFW and OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    // Generate and bind the texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Define the vertex and fragment shaders
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    // Create the shader program
    m_shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Set up the quad for rendering
    setupQuad();

    return true;
}

void Renderer::renderFrame(const uint8_t* frameData, int width, int height) {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    // std::cout<<"Display Width: "<<display_w<< " height: "<<display_h<<"\n";


    // Bind the texture and upload the frame data
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frameData);

    // Use the shader program and draw the quad
    glUseProgram(m_shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Render the ImGui UI
    render();

    // Swap buffers
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Renderer::render() {
    processInput();
    // Start a new ImGui frame
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    //
    // ImGui::Begin("Media Player Controls");
    // ImGui::Text("Hello, world!");
    // if (ImGui::Button("Play/Pause")) {
    //     // Add play/pause logic here
    // }
    // ImGui::End();
    //
    // // Render ImGui
    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::cleanup() {
    // Clean up ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Clean up OpenGL resources
    glDeleteTextures(1, &m_texture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(m_shaderProgram);

    // Destroy the m_window and terminate GLFW
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

GLuint Renderer::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for shader compile errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }

    return shader;
}

GLuint Renderer::createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    // Compile the vertex and fragment shaders
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    // Link the shaders into a program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }

    // Delete the shaders as they're linked into the program now
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Renderer::setupQuad() {
    // Define the vertices and texture coordinates for a full-screen quad
    float vertices[] = {
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Top right
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom left
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // Top left
    };

    // Define the indices for the quad (two triangles)
    unsigned int indices[] = {
        0, 1, 3, // First triangle
        1, 2, 3  // Second triangle
    };

    // Generate and bind the VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set up the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VAO
    glBindVertexArray(0);
}

void Renderer::processInput() const
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
}

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// The callbacks are updated and called BEFORE the Update loop is entered
// It can be assumed that inside the Update loop all callbacks have already been processed
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // For Dear ImGui to work it is necessary to queue if the mouse signal is already processed by Dear ImGui
    // Only if the mouse is not already captured it should be used here.
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
    }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    // For Dear ImGui to work it is necessary to queue if the mouse signal is already processed by Dear ImGui
    // Only if the mouse is not already captured it should be used here.
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int actions, int mods)
{
    // For Dear ImGui to work it is necessary to queue if the keyboard signal is already processed by Dear ImGui
    // Only if the keyboard is not already captured it should be used here.
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
}

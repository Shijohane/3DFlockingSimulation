#include "Application.h"

#include "Utility.h"
#include "RenderingPrimitives.h"

#include <glad/glad.h>
#include <iostream>

#define WIN_W 1280
#define WIN_H 720
#define WIN_W_MIN 800
#define WIN_H_MIN 600
#define WIN_W_MAX 1920
#define WIN_H_MAX 1080
#define WIN_TITLE "Flocking Simulation"

#pragma region window

Window::Window(void)
    : m_Window(nullptr) {}

Window::Window(int width, int height, const char *title)
    : m_Width(width), m_Height(height)
{
    // initialize glfw
    if(!glfwInit()) {
        std::cout << "WINDOW::ERROR: cannot initialize glfw" << std::endl;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window
    m_Width = Clamp(m_Width, WIN_W_MIN, WIN_W_MAX);
    m_Height = Clamp(m_Height, WIN_H_MIN, WIN_H_MAX);
    m_Window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!m_Window) {
        std::cout << "WINDOW::ERROR: cannot create window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowSizeLimits(m_Window, WIN_W_MIN, WIN_H_MIN, WIN_W_MAX, WIN_H_MAX);
    glfwSwapInterval(1);

    // load glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "WINDOW::ERROR: cannot initialize GLAD" << std::endl;
        return;
    }
    
    glViewport(0, 0, m_Width, m_Height);

    // print info
    std::cout << "WINDOW: successfully initialized window and OpenGL context" << std::endl;
    std::cout << "  GPU: (" << glGetString(GL_VENDOR) << ") " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "  OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
}

Window::~Window()
{
    // terminate glfw
    glfwTerminate();
}

void Window::SwapBuffers(void)
{
    glfwSwapBuffers(m_Window);
}

bool Window::WindowShouldClose(void)
{
    return glfwWindowShouldClose(m_Window);
}

int Window::GetWidth(void) const { return m_Width; }
int Window::GetHeight(void) const { return m_Height; }
GLFWwindow *Window::GetGLFWWindow(void) { return m_Window; }

#pragma endregion

#pragma region application

Application *Application::s_Instance = nullptr;
Application *Application::GetInstance(void)
{
    return s_Instance;
}

Application::Application(void)
{
    if(s_Instance == nullptr) {
        s_Instance = this;
    } else {
        std::cout << "APPLICATION::ERROR: creating second application" << std::endl;
        return;
    }

    m_Window = new Window(WIN_W, WIN_H, WIN_TITLE);
    m_Input.InitCallbacks();
}

Application::~Application()
{
    delete m_Window;

    s_Instance = nullptr;
}

void Application::Run(void)
{
    float vertices[] = {
        -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2
    };

    VertexArray vao;
    vao.Init();
    vao.Bind();

    VertexBuffer vbo;
    vbo.PushLayout(3);
    vbo.PushLayout(3);
    vbo.BufferData(vertices, 6 * 3);

    IndexBuffer ebo;
    ebo.BufferData(indices, 3);

    vao.Unbind();

    Shader shader;
    shader.InitShader("assets/shaders/Phong_V.glsl", "assets/shaders/Phong_F.glsl");

    while(!m_Window->WindowShouldClose()) {
        
        m_Input.PollEvents();
        
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.Bind();
        vao.Bind();
        ebo.Bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        vao.Unbind();
        m_Window->SwapBuffers();
    }
}

Window *Application::GetWindow(void)
{
    return m_Window;
}

#pragma endregion
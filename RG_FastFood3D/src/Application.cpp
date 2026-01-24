#include "../include/Renderer.h"
#include "../include/Application.h"
#include "../include/Gameplay.h"
#include "../include/Input.h"
#include <iostream>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Application::Application() {}
Application::~Application() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

bool Application::InitGLFW() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
    if (mode) { m_width = mode->width; m_height = mode->height; }

    m_window = glfwCreateWindow(m_width, m_height, "RG_FastFood3D", nullptr, nullptr);
    if (!m_window) return false;
    glfwMakeContextCurrent(m_window);
    return true;
}

bool Application::InitGLEW() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    glEnable(GL_DEPTH_TEST);
    return true;
}

int Application::Run() {
    if (!InitGLFW()) return -1;
    if (!InitGLEW()) return -1;

    Input::Init(m_window);

    m_renderer = std::make_unique<Renderer>(m_width, m_height);
    m_renderer->LoadShaders("shaders/basic.vert", "shaders/basic.frag");
    m_renderer->LoadUIShaders("shaders/rect.vert", "shaders/rect.frag");
    m_renderer->InitUIQuad();

    // Projection matrix
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.1f,
        100.0f
    );

    m_renderer->Use();
    m_renderer->SetProjection(projection);

    m_gameplay = std::make_unique<Gameplay>(m_renderer.get(), m_window);

    // Camera
    glm::vec3 camPos = glm::vec3(0, 5, 10);
    float yaw = -90.0f;
    float pitch = -20.0f;
    float camSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
    double lastMouseX = m_width * 0.5, lastMouseY = m_height * 0.5;
    bool firstMouse = true;

    m_lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window)) {

        double frameStart = glfwGetTime();
        double now = frameStart;
        double dt = now - m_lastTime;
        m_lastTime = now;
        float dtf = (float)dt;

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }

        if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS)
            m_renderer->EnableDepthTest(true);
        if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS)
            m_renderer->EnableDepthTest(false);
        if (glfwGetKey(m_window, GLFW_KEY_3) == GLFW_PRESS)
            m_renderer->EnableFaceCulling(true);
        if (glfwGetKey(m_window, GLFW_KEY_4) == GLFW_PRESS)
            m_renderer->EnableFaceCulling(false);

        glm::mat4 view;
        if (m_gameplay->GetState() == STATE_COOKING || m_gameplay->GetState() == STATE_ASSEMBLING) {
            // Camera movement
            if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)    camPos += camSpeed * dtf * glm::vec3(0, 0, -1);
            if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)  camPos += camSpeed * dtf * glm::vec3(0, 0, 1);
            if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)  camPos += camSpeed * dtf * glm::vec3(-1, 0, 0);
            if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) camPos += camSpeed * dtf * glm::vec3(1, 0, 0);

            // Camera view
            double mouseX, mouseY;
            glfwGetCursorPos(m_window, &mouseX, &mouseY);
            if (firstMouse) { lastMouseX = mouseX; lastMouseY = mouseY; firstMouse = false; }
            float offsetX = float(mouseX - lastMouseX) * mouseSensitivity;
            float offsetY = float(lastMouseY - mouseY) * mouseSensitivity;
            lastMouseX = mouseX;
            lastMouseY = mouseY;

            yaw += offsetX;
            pitch += offsetY;
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(front);

            view = glm::lookAt(camPos, camPos + front, glm::vec3(0, 1, 0));
        }
        else {
            // Fixed camera for start menu / Prijatno!
            camPos = glm::vec3(0, 5, 10);
            view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }

        m_renderer->Use();
        m_renderer->SetView(view);
        m_renderer->SetCameraPosition(camPos);
        m_gameplay->SetCamera(camPos, view, projection);

        m_gameplay->Update(dtf);

        glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_gameplay->OnRender();

        glfwSwapBuffers(m_window);
        glfwPollEvents();

        // Frame limiting
        double frameEnd = glfwGetTime();
        double elapsed = frameEnd - frameStart;
        if (elapsed < m_targetFrameSeconds) {
            double toSleep = m_targetFrameSeconds - elapsed;
            std::this_thread::sleep_for(std::chrono::duration<double>(toSleep));
        }
    }

    return 0;
}

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../include/AssemblingController.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "../include/Mesh.h"
#include <iostream>

class Renderer;

enum GameState { STATE_START_MENU, STATE_COOKING, STATE_ASSEMBLING, STATE_FINISHED };

class Gameplay {
public:
    Gameplay(Renderer* renderer, GLFWwindow* window);
    void Update(float dt);
    void OnRender();
    bool CheckPattieOvenCollision();

    void RenderFinalMessage();
    GLuint m_texPrijatno;
    int GetState() const { return m_state; }
    void SetCamera(const glm::vec3& camPos, const glm::mat4& view, const glm::mat4& proj);
    void UpdateCursor();

private:
    Renderer* m_renderer = nullptr;
    GLFWwindow* m_window = nullptr;
    GameState m_state = STATE_START_MENU;

    AssemblingController m_assembling;

    // resources
    GLuint m_texButton = 0;
    GLuint m_texCursor = 0;
    GLuint m_texPattieRaw = 0;
    GLuint m_texPattieCooked = 0;
    GLuint m_texStove = 0;
    GLuint m_texTable = 0;
    GLuint m_texSignature = 0;
    GLuint m_texGreen = 0;
    GLuint m_texGray = 0;

    // gameplay variables
    float m_btnPosX = 0.0f;
    float m_btnPosY = -0.4f;
    float m_btnScale = 0.35f;

    float m_signatureScale = 0.3f;

    float m_pattieX = 0.0f;
    float m_pattieY = 0.0f;
    const float m_pattieSpeed = 1.2f;
    float m_pattieScale = 0.25f;
    float m_cookProgress = 0.0f;
    const float m_cookRate = 0.4f;

    float m_stoveCenterX = 0.0f;
    float m_stoveCenterY = -0.6f;
    float m_stoveScale = 0.9f;
    float m_stoveHalfW = 0.6f, m_stoveHalfH = 0.25f;

    float m_barX = 0.0f;
    float m_barY = 0.80f;
    float m_barScale = 0.45f;
    float m_barThickness = 0.08f;

    bool m_leftPrev = false;

    glm::vec3 m_pattiePos = glm::vec3(0.0f, 3.55f, 4.0f);
    const float m_moveSpeed = 1.2f;

    GLuint m_pattieVAO = 0;
    int m_pattieVertexCount = 0;

    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::vec3 m_camPos;

    GLuint m_ovenVAO = 0;
    int m_ovenVertexCount = 0;

    bool m_cursorActive = true;

    std::unique_ptr<Mesh> m_pattieMesh;
    std::unique_ptr<Mesh> m_ovenMesh;
    std::unique_ptr<Mesh> m_tableMesh;

    glm::vec3 m_ovenMin;
    glm::vec3 m_ovenMax;

    glm::vec3 m_pattieSize;

    glm::vec3 m_tablePos = glm::vec3(0, -1.0f, 0);
};

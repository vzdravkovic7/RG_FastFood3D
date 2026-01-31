#include "../include/Gameplay.h"
#include "../include/Texture.h"
#include "../include/Input.h"
#include "../include/Renderer.h"
#include <iostream>
#include <cmath>

Gameplay::Gameplay(Renderer* renderer, GLFWwindow* window)
    : m_renderer(renderer), m_window(window)
{

    m_renderer->LoadShaders("shaders/pattie.vert", "shaders/pattie.frag");
    // load textures
    m_texButton = Texture::FromFile("res/button_order.png");
    m_texCursor = Texture::FromFile("res/cursor_spatula.png");
    m_texPattieRaw = Texture::FromFile("res/pattie_raw.png");
    m_texPattieCooked = Texture::FromFile("res/pattie.png");
    m_texStove = Texture::FromFile("res/stove.png");
    m_texTable = Texture::FromFile("res/table.png");
    m_texSignature = Texture::FromFile("res/signature.png");
    m_texGreen = Texture::FromFile("res/_solid_green.png");
    m_texGray = Texture::FromFile("res/_solid_gray.png");
    m_texPrijatno = Texture::FromFile("res/prijatno.png");
    m_texPlate = Texture::FromFile("res/plate.png");

    m_assembling.LoadTextures();
    m_pattieMesh = std::unique_ptr<Mesh>(Mesh::CreatePattie());
    m_assembling.LoadModels(m_pattieMesh.get());
    m_assembling.SetRenderer(m_renderer);

    if (m_texCursor)
        Input::InstallCursor(window, "res/cursor_spatula.png");

    m_ovenMesh = std::unique_ptr<Mesh>(Mesh::CreateOven());
    m_tableMesh = std::unique_ptr<Mesh>(Mesh::CreateTable());
    m_plateMesh = std::unique_ptr<Mesh>(Mesh::CreatePlate());

    m_ovenMin = glm::vec3(-0.5f, 0.0f, -1.7f);
    m_ovenMax = glm::vec3(0.5f, 0.5f, -0.7f);

    m_pattieSize = glm::vec3(0.75f, 0.125f, 0.75f);
}

void Gameplay::Update(float dt)
{
    UpdateCursor();

    int leftState = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
    bool leftNow = (leftState == GLFW_PRESS);

    if (leftNow && !m_leftPrev)
    {
        double mx, my;
        glfwGetCursorPos(m_window, &mx, &my);
        float ndcX = (float)((mx / m_renderer->GetWidth()) * 2.0 - 1.0);
        float ndcY = (float)(-((my / m_renderer->GetHeight()) * 2.0 - 1.0));

        if (m_state == STATE_START_MENU)
        {
            float bx0 = m_btnPosX - 0.2f * m_btnScale;
            float bx1 = m_btnPosX + 0.2f * m_btnScale;
            float by0 = m_btnPosY - 0.2f * m_btnScale;
            float by1 = m_btnPosY + 0.2f * m_btnScale;

            if (ndcX >= bx0 && ndcX <= bx1 &&
                ndcY >= by0 && ndcY <= by1)
            {
                m_state = STATE_COOKING;
                m_pattieX = 0.0f;
                m_pattieY = 0.0f;
                m_cookProgress = 0.0f;
                std::cout << "Order button clicked -> COOKING\n";
            }
        }
    }
    m_leftPrev = leftNow;

    // Cooking logic
    if (m_state == STATE_COOKING)
    {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) m_pattiePos.z -= m_moveSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) m_pattiePos.z += m_moveSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) m_pattiePos.x -= m_moveSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) m_pattiePos.x += m_moveSpeed * dt;

        if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) m_pattiePos.y += m_moveSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) m_pattiePos.y -= m_moveSpeed * dt;

        if (CheckPattieOvenCollision()) {
            m_cookProgress += dt * m_cookRate;
            if (m_cookProgress > 1.0f) m_cookProgress = 1.0f;
        }

        if (m_cookProgress >= 1.0f)
        {
            m_state = STATE_ASSEMBLING;
            m_assembling.Start(m_texPattieCooked);
            std::cout << "Cooking finished -> ASSEMBLING\n";
        }
    }

    if (m_state == STATE_ASSEMBLING)
    {
        m_assembling.Update(dt);

        if (m_assembling.IsFinished())
        {
            m_state = STATE_FINISHED;
            std::cout << "Burger completed -> FINISHED\n";
        }
    }
}

void Gameplay::OnRender()
{
    switch (m_state)
    {
    case STATE_START_MENU:
        m_renderer->DrawRect(m_texButton, m_btnPosX, m_btnPosY, m_btnScale);
        break;

    case STATE_COOKING:
    {
        m_renderer->RenderCookingScene(
            m_ovenMesh.get(),
            m_pattieMesh.get(),
            m_pattiePos,
            m_cookProgress,
            m_view,
            m_projection,
            m_camPos,
            m_texStove,
            m_texPattieRaw,
            m_texPattieCooked
        );

        // UI bar (2D)
        m_renderer->DrawRect(m_texGray, m_barX, m_barY, m_barScale, m_barThickness);

        float fillW = m_barScale * m_cookProgress;
        float leftEdge = m_barX - m_barScale * 0.5f;
        float greenX = leftEdge + fillW * 0.5f;

        m_renderer->DrawRect(m_texGreen, greenX, m_barY, fillW, m_barThickness * 0.85f);
    }
    break;

    case STATE_ASSEMBLING:
    {
        m_renderer->RenderAssemblingScene(
            m_tableMesh.get(),
            m_plateMesh.get(),
            m_view,
            m_projection,
            m_camPos,
            m_texPlate,
            m_texTable
        );
        m_assembling.Render(m_view, m_projection, m_camPos, false);
    }
    break;

    case STATE_FINISHED:
    {
        m_assembling.Render(m_view, m_projection, m_camPos, true);
        RenderFinalMessage();
    }
    break;
    }

    // Signature overlay
    m_renderer->SetAlpha(0.4f);
    m_renderer->DrawRect(m_texSignature, 0.75f, 0.75f, m_signatureScale, m_signatureScale);
    m_renderer->SetAlpha(1.0f);
}


void Gameplay::RenderFinalMessage()
{
    if (m_texPrijatno == 0) return;

    m_renderer->DrawRect(m_texPrijatno, 0.0f, 0.65f, 0.3f, 0.3f);
}

void Gameplay::SetCamera(const glm::vec3& camPos, const glm::mat4& view, const glm::mat4& proj) {
    m_camPos = camPos;
    m_view = view;
    m_projection = proj;
}

void Gameplay::UpdateCursor()
{
    if (!m_window) return;

    if (m_state == STATE_START_MENU || m_state == STATE_FINISHED)
    {
        if (!m_cursorActive)
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            Input::InstallCursor(m_window, "res/cursor_spatula.png");
            m_cursorActive = true;
        }
    }
    else
    {
        if (m_cursorActive)
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_cursorActive = false;
        }
    }
}

bool Gameplay::CheckPattieOvenCollision() {
    glm::vec3 pattieMin = m_pattiePos - m_pattieSize;
    glm::vec3 pattieMax = m_pattiePos + m_pattieSize;

    // Collision
    bool overlapX = pattieMax.x >= m_ovenMin.x && pattieMin.x <= m_ovenMax.x;
    bool overlapY = pattieMax.y >= m_ovenMin.y && pattieMin.y <= m_ovenMax.y;
    bool overlapZ = pattieMax.z >= m_ovenMin.z && pattieMin.z <= m_ovenMax.z;

    return overlapX && overlapY && overlapZ;
}


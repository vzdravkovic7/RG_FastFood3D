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

    m_assembling.LoadTextures();

    if (m_texCursor)
        Input::InstallCursor(window, "res/cursor_spatula.png");

    CreatePattieMesh();
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

        // Kretanje po Y (gore/dole)
        if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) m_pattiePos.y += m_moveSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) m_pattiePos.y -= m_moveSpeed * dt;

        bool touchingStove =
            (m_pattieX >= m_stoveCenterX - m_stoveHalfW) &&
            (m_pattieX <= m_stoveCenterX + m_stoveHalfW) &&
            (m_pattieY >= m_stoveCenterY - m_stoveHalfH) &&
            (m_pattieY <= m_stoveCenterY + m_stoveHalfH);

        if (touchingStove)
        {
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
        m_renderer->DrawRect(m_texStove, m_stoveCenterX, m_stoveCenterY, m_stoveScale);

        m_renderer->Use(); // Shader za pljeskavicu

        // --- Postavi matrice i poziciju kamere ---
        m_renderer->SetView(m_view);
        m_renderer->SetProjection(m_projection);
        m_renderer->SetCameraPosition(m_camPos);
        // Model matrica pljeskavice
        m_renderer->SetModel(glm::translate(glm::mat4(1.0f), m_pattiePos));
        Light light;
        light.pos = glm::vec3(0.0f, 3.0f, 2.0f);
        light.kA = glm::vec3(0.4f);
        light.kD = glm::vec3(0.9f);
        light.kS = glm::vec3(0.3f);

        m_renderer->SetLight(light);

        Material mat;
        mat.kA = glm::vec3(1.0f);
        mat.kD = glm::vec3(1.0f);
        mat.kS = glm::vec3(0.2f);
        mat.shine = 16.0f;

        m_renderer->SetMaterial(mat);


        // Teksture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texPattieRaw);
        glUniform1i(glGetUniformLocation(m_renderer->GetShaderID(), "uTexRaw"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texPattieCooked);
        glUniform1i(glGetUniformLocation(m_renderer->GetShaderID(), "uTexCooked"), 1);

        glUniform1f(glGetUniformLocation(m_renderer->GetShaderID(), "uBlend"), m_cookProgress);

        m_renderer->SetPattieTextures(
            m_texPattieRaw,
            m_texPattieCooked,
            m_cookProgress
        );

        // Render
        m_renderer->RenderMesh(m_pattieVAO, m_pattieVertexCount);

        // --- UI bar (2D sloj) ---
        m_renderer->DrawRect(m_texGray, m_barX, m_barY, m_barScale, m_barThickness);

        float fillW = m_barScale * m_cookProgress;
        float leftEdge = m_barX - m_barScale * 0.5f;
        float greenX = leftEdge + fillW * 0.5f;

        m_renderer->DrawRect(m_texGreen, greenX, m_barY, fillW, m_barThickness * 0.85f);
    }
    break;

    case STATE_ASSEMBLING:
    {
        m_renderer->DrawRect(m_texTable, 0.0f, -0.2f, 2.0f);
        // m_assembling.Render(*m_renderer);
    }
    break;

    case STATE_FINISHED:
    {
        // m_assembling.Render(*m_renderer);
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

    m_renderer->DrawRect(m_texPrijatno, 0.0f, 0.65f, 1.5f, 1.5f);
}

void Gameplay::CreatePattieMesh()
{
    std::vector<float> vertices; // pos(3) + normal(3) + uv(2)
    int segments = 32;
    float radius = 0.25f;
    float height = 0.05f;

    for (int i = 0; i < segments; i++)
    {
        float theta0 = 2.0f * 3.1415926f * float(i) / float(segments);
        float theta1 = 2.0f * 3.1415926f * float(i + 1) / float(segments);

        float cos0 = cos(theta0), sin0 = sin(theta0);
        float cos1 = cos(theta1), sin1 = sin(theta1);

        // --- Donja strana ---
        vertices.insert(vertices.end(), { 0, -height / 2, 0, 0, -1, 0, 0.5f, 0.5f });
        vertices.insert(vertices.end(), { radius * cos0, -height / 2, radius * sin0, 0, -1, 0, 0.5f + 0.5f * cos0, 0.5f + 0.5f * sin0 });
        vertices.insert(vertices.end(), { radius * cos1, -height / 2, radius * sin1, 0, -1, 0, 0.5f + 0.5f * cos1, 0.5f + 0.5f * sin1 });

        // --- Gornja strana ---
        vertices.insert(vertices.end(), { 0, height / 2, 0, 0, 1, 0, 0.5f, 0.5f });
        vertices.insert(vertices.end(), { radius * cos0, height / 2, radius * sin0, 0, 1, 0, 0.5f + 0.5f * cos0, 0.5f + 0.5f * sin0 });
        vertices.insert(vertices.end(), { radius * cos1, height / 2, radius * sin1, 0, 1, 0, 0.5f + 0.5f * cos1, 0.5f + 0.5f * sin1 });

        // --- Boène strane ---
        glm::vec3 n0 = glm::normalize(glm::vec3(cos0, 0, sin0));
        glm::vec3 n1 = glm::normalize(glm::vec3(cos1, 0, sin1));

        vertices.insert(vertices.end(), { radius * cos0, -height / 2, radius * sin0, n0.x, n0.y, n0.z, float(i) / segments, 0 });
        vertices.insert(vertices.end(), { radius * cos0, height / 2, radius * sin0, n0.x, n0.y, n0.z, float(i) / segments, 1 });
        vertices.insert(vertices.end(), { radius * cos1, height / 2, radius * sin1, n1.x, n1.y, n1.z, float(i + 1) / segments, 1 });

        vertices.insert(vertices.end(), { radius * cos0, -height / 2, radius * sin0, n0.x, n0.y, n0.z, float(i) / segments, 0 });
        vertices.insert(vertices.end(), { radius * cos1, height / 2, radius * sin1, n1.x, n1.y, n1.z, float(i + 1) / segments, 1 });
        vertices.insert(vertices.end(), { radius * cos1, -height / 2, radius * sin1, n1.x, n1.y, n1.z, float(i + 1) / segments, 0 });
    }

    m_pattieVertexCount = (int)vertices.size() / 8;

    // VAO + VBO
    GLuint VBO;
    glGenVertexArrays(1, &m_pattieVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(m_pattieVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // texcoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
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

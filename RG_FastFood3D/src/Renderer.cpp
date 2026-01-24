#include "../include/Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Renderer::Renderer(int w, int h)
    : m_shaderID(0), m_width(w), m_height(h)
{
}

Renderer::~Renderer() {
    if (m_shaderID != 0)
        glDeleteProgram(m_shaderID);
}

std::string Renderer::LoadFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cout << "Failed to open shader file: " << path << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint Renderer::LoadShader(GLenum type, const std::string& path) {
    std::string src = LoadFile(path);
    const char* csrc = src.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cout << "Shader compile error (" << path << "):\n" << info << std::endl;
    }

    return shader;
}

bool Renderer::LoadShaders(const std::string& vsPath, const std::string& fsPath) {
    GLuint vs = LoadShader(GL_VERTEX_SHADER, vsPath);
    GLuint fs = LoadShader(GL_FRAGMENT_SHADER, fsPath);

    m_shaderID = glCreateProgram();
    glAttachShader(m_shaderID, vs);
    glAttachShader(m_shaderID, fs);
    glLinkProgram(m_shaderID);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint success;
    glGetProgramiv(m_shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(m_shaderID, 512, nullptr, info);
        std::cout << "Shader link error:\n" << info << std::endl;
        return false;
    }

    m_locModel = glGetUniformLocation(m_shaderID, "uM");
    m_locView = glGetUniformLocation(m_shaderID, "uV");
    m_locProjection = glGetUniformLocation(m_shaderID, "uP");

    m_locViewPos = glGetUniformLocation(m_shaderID, "uViewPos");

    m_locLightPos = glGetUniformLocation(m_shaderID, "uLight.pos");
    m_locLightKA = glGetUniformLocation(m_shaderID, "uLight.kA");
    m_locLightKD = glGetUniformLocation(m_shaderID, "uLight.kD");
    m_locLightKS = glGetUniformLocation(m_shaderID, "uLight.kS");

    m_locMatKA = glGetUniformLocation(m_shaderID, "uMaterial.kA");
    m_locMatKD = glGetUniformLocation(m_shaderID, "uMaterial.kD");
    m_locMatKS = glGetUniformLocation(m_shaderID, "uMaterial.kS");
    m_locMatShine = glGetUniformLocation(m_shaderID, "uMaterial.shine");

    m_locTexRaw = glGetUniformLocation(m_shaderID, "uTexRaw");
    m_locTexCooked = glGetUniformLocation(m_shaderID, "uTexCooked");
    m_locBlend = glGetUniformLocation(m_shaderID, "uBlend");
    m_locBaseColor = glGetUniformLocation(m_shaderID, "uBaseColor");

    return true;
}

void Renderer::Use() {
    glUseProgram(m_shaderID);
}

void Renderer::SetModel(const glm::mat4& m) {
    glUniformMatrix4fv(m_locModel, 1, GL_FALSE, glm::value_ptr(m));
}

void Renderer::SetView(const glm::mat4& v) {
    glUniformMatrix4fv(m_locView, 1, GL_FALSE, glm::value_ptr(v));
}

void Renderer::SetProjection(const glm::mat4& p) {
    glUniformMatrix4fv(m_locProjection, 1, GL_FALSE, glm::value_ptr(p));
}

void Renderer::SetLight(const Light& l) {
    glUniform3fv(m_locLightPos, 1, glm::value_ptr(l.pos));
    glUniform3fv(m_locLightKA, 1, glm::value_ptr(l.kA));
    glUniform3fv(m_locLightKD, 1, glm::value_ptr(l.kD));
    glUniform3fv(m_locLightKS, 1, glm::value_ptr(l.kS));
}

void Renderer::SetMaterial(const Material& m) {
    glUniform3fv(m_locMatKA, 1, glm::value_ptr(m.kA));
    glUniform3fv(m_locMatKD, 1, glm::value_ptr(m.kD));
    glUniform3fv(m_locMatKS, 1, glm::value_ptr(m.kS));
    glUniform1f(m_locMatShine, m.shine);
}

void Renderer::SetCameraPosition(const glm::vec3& pos) {
    glUniform3fv(m_locViewPos, 1, glm::value_ptr(pos));
}

void Renderer::RenderMesh(GLuint vao, int vertexCount) {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

bool Renderer::LoadUIShaders(const std::string& vs, const std::string& fs)
{
    GLuint v = LoadShader(GL_VERTEX_SHADER, vs);
    GLuint f = LoadShader(GL_FRAGMENT_SHADER, fs);

    m_uiShader = glCreateProgram();
    glAttachShader(m_uiShader, v);
    glAttachShader(m_uiShader, f);
    glLinkProgram(m_uiShader);

    glDeleteShader(v);
    glDeleteShader(f);

    GLint success;
    glGetProgramiv(m_uiShader, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(m_uiShader, 512, nullptr, info);
        std::cout << "UI shader link error:\n" << info << std::endl;
        return false;
    }

    m_uiLocPos = glGetUniformLocation(m_uiShader, "uPos");
    m_uiLocScale = glGetUniformLocation(m_uiShader, "uScale");
    m_uiLocAlpha = glGetUniformLocation(m_uiShader, "uAlpha");
    m_uiLocUseTex1 = glGetUniformLocation(m_uiShader, "useTex1");
    m_uiLocBlend = glGetUniformLocation(m_uiShader, "uBlend");

    return true;
}

void Renderer::InitUIQuad()
{
    float verts[] = {
        -1, -1, 0, 0,
         1, -1, 1, 0,
         1,  1, 1, 1,
        -1,  1, 0, 1
    };

    GLuint vbo;
    glGenVertexArrays(1, &m_uiVAO);
    glGenBuffers(1, &vbo);

    glBindVertexArray(m_uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::DrawRect(GLuint tex, float x, float y, float sx, float sy)
{
    if (sy < 0.0f) sy = sx;

    glUseProgram(m_uiShader);
    glDisable(GL_DEPTH_TEST);

    glUniform2f(m_uiLocPos, x, y);
    glUniform2f(m_uiLocScale, sx, sy);
    glUniform1f(m_uiLocAlpha, 1.0f);
    glUniform1i(m_uiLocUseTex1, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glBindVertexArray(m_uiVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::DrawRectBlend(GLuint baseTex, float x, float y, float s, GLuint tex1, float blend)
{
    glUseProgram(m_uiShader);
    glDisable(GL_DEPTH_TEST);

    glUniform2f(m_uiLocPos, x, y);
    glUniform2f(m_uiLocScale, s, s);
    glUniform1f(m_uiLocAlpha, 1.0f);

    if (tex1 != 0)
    {
        glUniform1i(m_uiLocUseTex1, 1);
        glUniform1f(m_uiLocBlend, blend);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, baseTex);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
    }
    else
    {
        glUniform1i(m_uiLocUseTex1, 0);
    }

    glBindVertexArray(m_uiVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::SetAlpha(float a)
{
    glUseProgram(m_uiShader);
    glUniform1f(m_uiLocAlpha, a);
}

void Renderer::SetPattieTextures(GLuint rawTex, GLuint cookedTex, float blend)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rawTex);
    glUniform1i(m_locTexRaw, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cookedTex);
    glUniform1i(m_locTexCooked, 1);

    glUniform1f(m_locBlend, blend);

    glUniform3f(m_locBaseColor, 0.45f, 0.22f, 0.15f);
}

void Renderer::EnableDepthTest(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void Renderer::EnableFaceCulling(bool enable)
{
    if (enable)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer::RenderCookingScene(
    Mesh* oven,
    Mesh* pattie,
    const glm::vec3& pattiePos,
    float cookProgress,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& camPos,
    GLuint texStove,
    GLuint texPattieRaw,
    GLuint texPattieCooked
)
{
    Use();

    SetView(view);
    SetProjection(projection);
    SetCameraPosition(camPos);

    Light light;
    light.pos = glm::vec3(0.0f, 3.0f, 2.0f);
    light.kA = glm::vec3(0.4f);
    light.kD = glm::vec3(0.9f);
    light.kS = glm::vec3(0.3f);
    SetLight(light);

    Material mat;
    mat.kA = glm::vec3(1.0f);
    mat.kD = glm::vec3(1.0f);
    mat.kS = glm::vec3(0.2f);
    mat.shine = 16.0f;
    SetMaterial(mat);

    glm::mat4 Moven = glm::mat4(1.0f);
    Moven = glm::translate(Moven, glm::vec3(0.0f, 0.0f, -1.2f));
    SetModel(Moven);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texStove);
    glUniform1i(glGetUniformLocation(m_shaderID, "uTexRaw"), 0);
    glUniform1i(glGetUniformLocation(m_shaderID, "uTexCooked"), 0);
    glUniform1f(glGetUniformLocation(m_shaderID, "uBlend"), 0.0f);

    if (oven) oven->Draw();

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::translate(M, pattiePos);
    M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    M = glm::scale(M, glm::vec3(1.5f, 1.5f, 0.25f));
    SetModel(M);

    SetPattieTextures(texPattieRaw, texPattieCooked, cookProgress);

    if (pattie) pattie->Draw();
}

void Renderer::RenderTable(
    Mesh* tableMesh,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& camPos,
    GLuint texTable
)
{
    Use();

    SetView(view);
    SetProjection(projection);
    SetCameraPosition(camPos);

    Light light;
    light.pos = glm::vec3(0.0f, 3.0f, 2.0f);
    light.kA = glm::vec3(0.4f);
    light.kD = glm::vec3(0.9f);
    light.kS = glm::vec3(0.3f);
    SetLight(light);

    Material mat;
    mat.kA = glm::vec3(1.0f);
    mat.kD = glm::vec3(1.0f);
    mat.kS = glm::vec3(0.2f);
    mat.shine = 16.0f;
    SetMaterial(mat);

    glm::mat4 M = glm::mat4(1.0f);
    M = glm::translate(M, glm::vec3(0.0f, -0.2f, 0.0f));
    M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));
    SetModel(M);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texTable);
    glUniform1i(glGetUniformLocation(m_shaderID, "uTexRaw"), 0);
    glUniform1i(glGetUniformLocation(m_shaderID, "uTexCooked"), 0);
    glUniform1f(glGetUniformLocation(m_shaderID, "uBlend"), 0.0f);

    if (tableMesh)
        tableMesh->Draw();
}

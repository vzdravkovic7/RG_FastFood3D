#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "../include/Mesh.h"
#include "AssemblingController.h"
#include <iostream>

struct Spill;

struct Light {
    glm::vec3 pos;
    glm::vec3 kA;
    glm::vec3 kD;
    glm::vec3 kS;
    bool enabled = true;
};

struct Material {
    glm::vec3 kA;
    glm::vec3 kD;
    glm::vec3 kS;
    float shine;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool LoadShaders(const std::string& vsPath, const std::string& fsPath);
    void Use();
    void UseTexturedShader();
    bool LoadTexturedShaders(const std::string& vs, const std::string& fs);

    // MVP matrice
    void SetModel(const glm::mat4& m);
    void SetView(const glm::mat4& v);
    void SetProjection(const glm::mat4& p);

    // Light, Material, Camera
    void SetLight(const Light& l);
    void SetMaterial(const Material& m);
    void SetMeshMaterial(Mesh* mesh);
    void SetCameraPosition(const glm::vec3& pos);

    void RenderMesh(GLuint vao, int vertexCount);

    bool LoadUIShaders(const std::string& vs, const std::string& fs);
    void InitUIQuad();

    void DrawRect(GLuint tex, float x, float y, float sx, float sy = -1.0f);
    void DrawRectBlend(GLuint baseTex, float x, float y, float s, GLuint tex1, float blend);
    void SetAlpha(float a);
    void SetPattieTextures(GLuint rawTex, GLuint cookedTex, float blend);

    Renderer(int w, int h);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetShaderID() const { return m_shaderID; }
    int GetTexturedShaderID() const { return m_texturedShaderID; }

    void EnableDepthTest(bool enable);
    void EnableFaceCulling(bool enable);

    void RenderCookingScene(
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
    );

    void RenderAssemblingScene(
        Mesh* table,
        Mesh* plate,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& camPos,
        GLuint texPlate,
        GLuint texTable
    );

    void RenderSpill(
        Mesh* quad,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& camPos,
        const Spill& s);

    void RenderIngredient(Mesh* mesh,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& camPos,
        GLuint texture,
        const glm::vec3& pos,
        const glm::vec3& scale,
        const glm::vec3& rot);

    void ToggleLight();
    void SetLightEnabled(bool enabled);
    bool IsLightEnabled() const;

private:
    int m_width;
    int m_height;

    GLuint m_shaderID;
    GLuint m_texturedShaderID;
    GLint m_locTexSingle;
    GLint m_locHasTexture;
    GLint m_locFlatColor;

    GLint m_locModel;
    GLint m_locView;
    GLint m_locProjection;
    GLint m_locViewPos;

    GLint m_locLightPos;
    GLint m_locLightKA;
    GLint m_locLightKD;
    GLint m_locLightKS;

    GLint m_locMatKA;
    GLint m_locMatKD;
    GLint m_locMatKS;
    GLint m_locMatShine;

    GLuint m_uiShader = 0;
    GLuint m_uiVAO = 0;
    GLint m_uiLocPos = -1;
    GLint m_uiLocScale = -1;
    GLint m_uiLocAlpha = -1;
    GLint m_uiLocUseTex1 = -1;
    GLint m_uiLocBlend = -1;

private:
    GLuint LoadShader(GLenum type, const std::string& path);
    std::string LoadFile(const std::string& path);

    GLint m_locTexRaw;
    GLint m_locTexCooked;
    GLint m_locBlend;
    GLint m_locBaseColor;
    Light m_light;
};

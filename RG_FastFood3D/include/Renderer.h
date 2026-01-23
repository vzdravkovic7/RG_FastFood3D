#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

struct Light {
    glm::vec3 pos;
    glm::vec3 kA;
    glm::vec3 kD;
    glm::vec3 kS;
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

    // MVP matrice
    void SetModel(const glm::mat4& m);
    void SetView(const glm::mat4& v);
    void SetProjection(const glm::mat4& p);

    // Light, Material, Camera
    void SetLight(const Light& l);
    void SetMaterial(const Material& m);
    void SetCameraPosition(const glm::vec3& pos);

    // Crtanje
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

    void EnableDepthTest(bool enable);
    void EnableFaceCulling(bool enable);

private:
    int m_width;
    int m_height;

    GLuint m_shaderID;

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
};

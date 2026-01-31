#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Mesh;

enum IngredientType {
    ING_BUN_BOTTOM,
    ING_PATTIE,
    ING_KETCHUP,
    ING_MUSTARD,
    ING_PICKLES,
    ING_ONION,
    ING_LETTUCE,
    ING_CHEESE,
    ING_TOMATO,
    ING_BUN_TOP
};

class Ingredient {
public:
    Ingredient(IngredientType type, Mesh* mesh, GLuint textureID);

    void ResetPosition();
    void Update(float dt);
    void Render(class Renderer* renderer,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& camPos);

    bool IsPlaced() const { return m_placed; }
    void MarkPlaced() { m_placed = true; }

    void ForcePosition(float x, float z);

    IngredientType GetType() const { return m_type; }
    Mesh* GetMesh() const { return m_mesh; }
    GLuint GetTexture() const { return m_texture; }

    glm::vec3 GetPosition() const { return m_position; }
    void SetScale(const glm::vec3& s) { m_scale = s; }
    glm::vec3 GetScale() const { return m_scale; }
    glm::vec3 GetRotation() const { return m_rotation; }

    void Show() { m_visible = true; }
    void Hide() { m_visible = false; }
    bool IsVisible() const { return m_visible; }

    void SetY(float y) { m_position.y = y; }

    glm::vec3 GetTipWorldPosition() const;

private:
    IngredientType m_type;

    Mesh* m_mesh;
    GLuint m_texture;

    glm::vec3 m_position;
    glm::vec3 m_scale = glm::vec3(1.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);

    float m_speed = 1.2f;
    bool m_placed = false;
    bool m_visible = true;
};

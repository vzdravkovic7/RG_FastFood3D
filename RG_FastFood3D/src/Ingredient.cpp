#include "../include/Ingredient.h"
#include "../include/Input.h"
#include "../include/Renderer.h"
#include <glm/gtc/type_ptr.hpp>

Ingredient::Ingredient(IngredientType type, Mesh* mesh, GLuint textureID)
    : m_type(type), m_mesh(mesh), m_texture(textureID)
{
    ResetPosition();
}

void Ingredient::ResetPosition() {
    m_position = glm::vec3(0.0f, 0.5f, 0.0f);

    switch (m_type)
    {
    case ING_BUN_BOTTOM:
        SetScale(glm::vec3(0.25f));
        break;

    case ING_PATTIE:
        SetScale(glm::vec3(0.9f, 0.7f, 0.9f));
        m_rotation.x = glm::radians(-90.0f);
        break;

    case ING_KETCHUP:
        m_rotation.x = glm::radians(90.0f);
        break;

    case ING_MUSTARD:
        m_rotation.x = glm::radians(90.0f);
        break;

    case ING_ONION:
        SetScale(glm::vec3(0.35f));
        m_rotation.x = glm::radians(90.0f);
        break;

    case ING_PICKLES:
        SetScale(glm::vec3(0.25f));
        break;

    case ING_LETTUCE:
        SetScale(glm::vec3(0.15f));
        break;

    case ING_CHEESE:
        SetScale(glm::vec3(0.25f));
        break;

    case ING_TOMATO:
        SetScale(glm::vec3(0.15f));
        break;

    case ING_BUN_TOP:
        SetScale(glm::vec3(0.22f));
        break;

    default:
        SetScale(glm::vec3(1.0f));
        break;
    }
}

void Ingredient::Update(float dt) {
    if (m_placed) return;

    if (Input::KeyDown(GLFW_KEY_W)) m_position.z -= m_speed * dt;
    if (Input::KeyDown(GLFW_KEY_S)) m_position.z += m_speed * dt;
    if (Input::KeyDown(GLFW_KEY_A)) m_position.x -= m_speed * dt;
    if (Input::KeyDown(GLFW_KEY_D)) m_position.x += m_speed * dt;

    if (Input::KeyDown(GLFW_KEY_SPACE)) m_position.y += m_speed * dt;
    if (Input::KeyDown(GLFW_KEY_LEFT_SHIFT)) m_position.y -= m_speed * dt;
}

void Ingredient::Render(Renderer* renderer,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& camPos)
{
    if (!m_visible) return;

    renderer->RenderIngredient(
        m_mesh,
        view,
        projection,
        camPos,
        m_texture,
        m_position,
        m_scale,
        m_rotation
    );
}

void Ingredient::ForcePosition(float x, float z) {
    m_position.x = x;
    m_position.z = z;
}

glm::vec3 Ingredient::GetTipWorldPosition() const
{
    glm::vec3 localTip(0.0f);

    if (m_type == ING_KETCHUP || m_type == ING_MUSTARD)
    {
        localTip = glm::vec3(0.0f, 0.6f, 0.0f);
    }

    localTip *= m_scale;

    // Rotation
    glm::mat4 rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, m_rotation.x, glm::vec3(1, 0, 0));
    rot = glm::rotate(rot, m_rotation.y, glm::vec3(0, 1, 0));
    rot = glm::rotate(rot, m_rotation.z, glm::vec3(0, 0, 1));

    glm::vec3 rotatedTip = glm::vec3(rot * glm::vec4(localTip, 1.0f));

    // World space
    return m_position + rotatedTip;
}

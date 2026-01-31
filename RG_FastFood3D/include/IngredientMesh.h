#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <GL/glew.h>

class IngredientMesh {
public:
    Mesh* mesh = nullptr;
    GLuint texture = 0;
    glm::vec3 position{ 0,0,0 };
    glm::vec3 scale{ 1,1,1 };
    glm::vec3 rotationEuler{ 0,0,0 };
};

#pragma once
#include <vector>
#include "Ingredient.h"
#include "Plate.h"
#include <iostream>

class Renderer;

struct Spill {
    GLuint tex;
    glm::vec3 pos;
    float scale;
};

class AssemblingController {
public:
    AssemblingController();

    void LoadTextures();
    void LoadModels(Mesh* pattie);
    void SpawnSpill(GLuint texID, float x, float y, float scale);
    void Start(GLuint cookedPattieTexture);

    void Update(float dt);
    void Render(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& camPos,
        bool finished);
    void SetRenderer(Renderer* r) { m_renderer = r; }

    bool IsFinished() const { return m_done; }

    float ConvertScreenToX(float screenX);
    float ConvertScreenToZ(float screenY);


private:
    std::vector<Spill> m_spills;

    std::vector<Ingredient> m_list;
    int m_currentIndex = 0;
    bool m_done = false;

    Renderer* m_renderer = nullptr;

    // textures for all ingredients
    GLuint m_texBunBottom = 0;
    GLuint m_texPattie = 0;
    GLuint m_texKetchup = 0;
    GLuint m_texMustard = 0;
    GLuint m_texPickles = 0;
    GLuint m_texOnion = 0;
    GLuint m_texLettuce = 0;
    GLuint m_texCheese = 0;
    GLuint m_texTomato = 0;
    GLuint m_texBunTop = 0;

    float m_stackOffsetY = 0.0f;

    bool IngredientOverPlate(const Ingredient& ing) const;

    GLuint m_texKetchupSpill = 0;
    GLuint m_texMustardSpill = 0;

    Mesh* m_meshPlate = nullptr;
    Mesh* m_meshSpill = nullptr;

    std::unique_ptr<Mesh> m_meshBunBottom;
    Mesh* m_meshPattie = nullptr;
    std::unique_ptr<Mesh> m_meshKetchup;
    std::unique_ptr<Mesh> m_meshMustard;
    std::unique_ptr<Mesh> m_meshPickles;
    std::unique_ptr<Mesh> m_meshOnion;
    std::unique_ptr<Mesh> m_meshLettuce;
    std::unique_ptr<Mesh> m_meshCheese;
    std::unique_ptr<Mesh> m_meshTomato;
    std::unique_ptr<Mesh> m_meshBunTop;
};

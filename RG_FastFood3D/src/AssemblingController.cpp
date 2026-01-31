#include "../include/AssemblingController.h"
#include "../include/Texture.h"
#include "../include/Input.h"
#include <iostream>
#include "../include/ModelLoader.h"

AssemblingController::AssemblingController() {}

void AssemblingController::LoadTextures() {
    m_texBunBottom = Texture::FromFile("res/bottom_bun.png");
    m_texPattie = Texture::FromFile("res/pattie.png");
    m_texKetchup = Texture::FromFile("res/ketchup_bottle.png");
    m_texMustard = Texture::FromFile("res/mustard_bottle.png");
    m_texPickles = Texture::FromFile("res/pickles.png");
    m_texOnion = Texture::FromFile("res/onion_ring.png");
    m_texLettuce = Texture::FromFile("res/lettuce.png");
    m_texCheese = Texture::FromFile("res/cheese.png");
    m_texTomato = Texture::FromFile("res/tomato.png");
    m_texBunTop = Texture::FromFile("res/top_bun.png");
    m_texKetchupSpill = Texture::FromFile("res/ketchup_spill.png");
    m_texMustardSpill = Texture::FromFile("res/mustard_spill.png");

    m_meshSpill = Mesh::CreateSpillQuad();
}

void AssemblingController::LoadModels(Mesh* pattie) {
    m_meshBunBottom = std::unique_ptr<Mesh>(ModelLoader::LoadOBJ("models/buns/bun_bottom.obj"));
    m_meshPattie = pattie;
    m_meshKetchup = std::unique_ptr<Mesh>(Mesh::CreateSauceBottle());
    m_meshKetchup->SetTexture(m_texKetchup);
    m_meshMustard = std::unique_ptr<Mesh>(Mesh::CreateSauceBottle());
    m_meshMustard->SetTexture(m_texMustard);
    m_meshPickles = std::unique_ptr<Mesh>(ModelLoader::LoadOBJ("models/pickles/krastavac.obj"));
    m_meshOnion = std::unique_ptr<Mesh>(ModelLoader::LoadOBJ("models/onion/luk.obj"));
    m_meshLettuce = std::unique_ptr<Mesh>(ModelLoader::LoadOBJ("models/lettuce/salata.obj"));
    m_meshCheese = std::unique_ptr<Mesh>(ModelLoader::LoadOBJ("models/cheese/sir.obj"));
    m_meshTomato = std::unique_ptr<Mesh>(ModelLoader::LoadOBJ("models/tomato/paradajz.obj"));
    m_meshBunTop = std::unique_ptr<Mesh>(ModelLoader::LoadOBJ("models/buns/bun_top.obj"));
}

void AssemblingController::SpawnSpill(GLuint texID, float x, float z, float scale)
{
    Spill s;
    s.tex = texID;
    s.pos = glm::vec3(x, -0.49f, z);
    s.scale = scale;
    m_spills.push_back(s);
}

void AssemblingController::Start(GLuint cookedPattieTexture) {
    m_list.clear();

    m_list.emplace_back(ING_BUN_BOTTOM, m_meshBunBottom.get(), m_meshBunBottom->GetTexture());
    m_list.emplace_back(ING_PATTIE, m_meshPattie, cookedPattieTexture);
    m_list.emplace_back(ING_KETCHUP, m_meshKetchup.get(), m_meshKetchup->GetTexture());
    m_list.emplace_back(ING_MUSTARD, m_meshMustard.get(), m_meshMustard->GetTexture());
    m_list.emplace_back(ING_PICKLES, m_meshPickles.get(), m_meshPickles->GetTexture());
    m_list.emplace_back(ING_ONION, m_meshOnion.get(), m_meshOnion->GetTexture());
    m_list.emplace_back(ING_LETTUCE, m_meshLettuce.get(), m_meshLettuce->GetTexture());
    m_list.emplace_back(ING_CHEESE, m_meshCheese.get(), m_meshCheese->GetTexture());
    m_list.emplace_back(ING_TOMATO, m_meshTomato.get(), m_meshTomato->GetTexture());
    m_list.emplace_back(ING_BUN_TOP, m_meshBunTop.get(), m_meshBunTop->GetTexture());

    m_currentIndex = 0;
    m_done = false;

    // Plate setup
    float plateX = 0.0f;
    float plateY = -0.35f;
    float plateScale = 0.7f;

    m_stackOffsetY = plateY + 0.2f;

    for (auto& ing : m_list)
        ing.Hide();

    if (!m_list.empty())
        m_list[0].Show();
}

bool AssemblingController::IngredientOverPlate(const Ingredient& ing) const
{
    glm::vec3 platePos(0.0f, -0.25f, 0.0f);

    glm::vec3 p = ing.GetPosition();

    float dx = p.x - platePos.x;
    float dz = p.z - platePos.z;

    float distSq = dx * dx + dz * dz;

    float plateRadius = 0.35f;

    return distSq <= plateRadius * plateRadius;
}

void AssemblingController::Update(float dt) {
    if (m_done) return;

    Ingredient& cur = m_list[m_currentIndex];
    if (!cur.IsPlaced())
        cur.Update(dt);

    bool isSauce = (cur.GetType() == ING_KETCHUP || cur.GetType() == ING_MUSTARD);

    if (Input::KeyPressed(GLFW_KEY_ENTER)) {

        //  SAUCE LOGIC
        if (isSauce)
        {
            glm::vec3 tip = cur.GetTipWorldPosition();

            GLuint spillTex = (cur.GetType() == ING_KETCHUP)
                ? m_texKetchupSpill
                : m_texMustardSpill;

            // Plate check
            glm::vec3 platePos(0.0f, -0.25f, 0.0f);

            float dx = tip.x - platePos.x;
            float dz = tip.z - platePos.z;

            float distSq = dx * dx + dz * dz;
            float plateRadius = 0.35f;

            if (distSq <= plateRadius * plateRadius)
            {
                SpawnSpill(spillTex, 0.0f, 4.0f, 0.22f);

                cur.Hide();
                cur.MarkPlaced();
                m_currentIndex++;

                if (m_currentIndex < (int)m_list.size())
                    m_list[m_currentIndex].Show();
                else
                    m_done = true;

                return;
            }

            // Table check
            if (tip.y < -0.35f)
            {
                SpawnSpill(spillTex, tip.x, tip.z + 4.0f, 0.18f);

                return;
            }

            return;
        }

        //  STANDARD INGREDIENT
        if (!IngredientOverPlate(cur))
            return;

        cur.ForcePosition(0.0f, 0.0f);
        cur.SetY(m_stackOffsetY);
        cur.MarkPlaced();
        m_stackOffsetY += 0.07f;
        m_currentIndex++;

        if (m_currentIndex < (int)m_list.size())
        {
            m_list[m_currentIndex].Show();
        }
        else
        {
            m_done = true;
        }
    }
}

void AssemblingController::Render(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& camPos,
    bool finished) {

    for (const Spill& s : m_spills)
        m_renderer->RenderSpill(m_meshSpill, view, projection, camPos, s);

    float finishedScaleMul = finished ? 2.0f : 1.0f;

    for (auto& ing : m_list)
    {
        if (!ing.IsVisible() && !ing.IsPlaced())
            continue;

        if ((ing.GetType() == ING_KETCHUP || ing.GetType() == ING_MUSTARD) && ing.IsPlaced())
            continue;

        glm::vec3 originalScale = ing.GetScale();

        if (finished)
            ing.SetScale(originalScale * finishedScaleMul);

        ing.Render(m_renderer, view, projection, camPos);

        if (finished)
            ing.SetScale(originalScale);
    }
}

float AssemblingController::ConvertScreenToX(float screenX)
{
    return screenX * 0.3f;
}

float AssemblingController::ConvertScreenToZ(float screenY)
{
    return screenY * 0.5f;
}

#include "../include/Mesh.h"
#include <cmath>

Mesh::Mesh(const std::vector<Vertex>& vertices)
{
    vertexCount = vertices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Mesh::Draw() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

Mesh* Mesh::CreatePattie()
{
    std::vector<Vertex> vertices;
    int segments = 32;
    float radius = 0.25f;
    float height = 0.05f;

    for (int i = 0; i < segments; i++)
    {
        float theta0 = 2.0f * 3.1415926f * float(i) / float(segments);
        float theta1 = 2.0f * 3.1415926f * float(i + 1) / float(segments);

        float cos0 = cos(theta0), sin0 = sin(theta0);
        float cos1 = cos(theta1), sin1 = sin(theta1);

        // Donja strana
        vertices.push_back({ {0,-height / 2,0}, {0,-1,0}, {0.5f,0.5f} });
        vertices.push_back({ {radius * cos0,-height / 2,radius * sin0}, {0,-1,0}, {0.5f + 0.5f * cos0,0.5f + 0.5f * sin0} });
        vertices.push_back({ {radius * cos1,-height / 2,radius * sin1}, {0,-1,0}, {0.5f + 0.5f * cos1,0.5f + 0.5f * sin1} });

        // Gornja strana
        vertices.push_back({ {0,height / 2,0}, {0,1,0}, {0.5f,0.5f} });
        vertices.push_back({ {radius * cos0,height / 2,radius * sin0}, {0,1,0}, {0.5f + 0.5f * cos0,0.5f + 0.5f * sin0} });
        vertices.push_back({ {radius * cos1,height / 2,radius * sin1}, {0,1,0}, {0.5f + 0.5f * cos1,0.5f + 0.5f * sin1} });

        // Boène strane
        glm::vec3 n0 = glm::normalize(glm::vec3(cos0, 0, sin0));
        glm::vec3 n1 = glm::normalize(glm::vec3(cos1, 0, sin1));

        vertices.push_back({ {radius * cos0,-height / 2,radius * sin0}, n0, {float(i) / segments,0} });
        vertices.push_back({ {radius * cos0,height / 2,radius * sin0}, n0, {float(i) / segments,1} });
        vertices.push_back({ {radius * cos1,height / 2,radius * sin1}, n1, {float(i + 1) / segments,1} });

        vertices.push_back({ {radius * cos0,-height / 2,radius * sin0}, n0, {float(i) / segments,0} });
        vertices.push_back({ {radius * cos1,height / 2,radius * sin1}, n1, {float(i + 1) / segments,1} });
        vertices.push_back({ {radius * cos1,-height / 2,radius * sin1}, n1, {float(i + 1) / segments,0} });
    }

    return new Mesh(vertices);
}

Mesh* Mesh::CreateOven()
{
    std::vector<Vertex> vertices;

    auto PushQuad = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n) {
        vertices.push_back({ a,n,{0,0} });
        vertices.push_back({ b,n,{1,0} });
        vertices.push_back({ c,n,{1,1} });
        vertices.push_back({ a,n,{0,0} });
        vertices.push_back({ c,n,{1,1} });
        vertices.push_back({ d,n,{0,1} });
        };

    float W = 1.0f, H = 0.6f, Ddim = 0.6f;
    float legW = 0.12f, legH = 0.45f;

    glm::vec3 A = { -W / 2, 0, -Ddim / 2 };
    glm::vec3 B = { W / 2, 0, -Ddim / 2 };
    glm::vec3 C = { W / 2, H, -Ddim / 2 };
    glm::vec3 D = { -W / 2, H, -Ddim / 2 };
    glm::vec3 E = { -W / 2, 0, Ddim / 2 };
    glm::vec3 F = { W / 2, 0, Ddim / 2 };
    glm::vec3 G = { W / 2, H, Ddim / 2 };
    glm::vec3 Ht = { -W / 2, H, Ddim / 2 };

    PushQuad(A, B, C, D, { 0,0,-1 });
    PushQuad(E, F, G, Ht, { 0,0,1 });
    PushQuad(A, E, Ht, D, { -1,0,0 });
    PushQuad(B, F, G, C, { 1,0,0 });
    PushQuad(D, C, G, Ht, { 0,1,0 });
    PushQuad(A, B, F, E, { 0,-1,0 });

    auto AddLeg = [&](float ox, float oz) {
        float x0 = ox - legW / 2, x1 = ox + legW / 2;
        float y0 = -legH, y1 = 0;
        float z0 = oz - legW / 2, z1 = oz + legW / 2;

        glm::vec3 L1 = { x0,y0,z0 }; glm::vec3 L2 = { x1,y0,z0 };
        glm::vec3 L3 = { x1,y1,z0 }; glm::vec3 L4 = { x0,y1,z0 };
        glm::vec3 R1 = { x0,y0,z1 }; glm::vec3 R2 = { x1,y0,z1 };
        glm::vec3 R3 = { x1,y1,z1 }; glm::vec3 R4 = { x0,y1,z1 };

        PushQuad(L1, L2, L3, L4, { 0,0,-1 });
        PushQuad(R1, R2, R3, R4, { 0,0,1 });
        PushQuad(L1, R1, R4, L4, { -1,0,0 });
        PushQuad(L2, R2, R3, L3, { 1,0,0 });
        PushQuad(L4, L3, R3, R4, { 0,1,0 });
        PushQuad(L1, L2, R2, R1, { 0,-1,0 });
        };

    AddLeg(-W / 2 + legW, -Ddim / 2 + legW);
    AddLeg(W / 2 - legW, -Ddim / 2 + legW);
    AddLeg(-W / 2 + legW, Ddim / 2 - legW);
    AddLeg(W / 2 - legW, Ddim / 2 - legW);

    return new Mesh(vertices);
}

Mesh* Mesh::CreateTable()
{
    std::vector<Vertex> vertices;

    auto PushQuad = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n) {
        vertices.push_back({ a,n,{0,0} });
        vertices.push_back({ b,n,{1,0} });
        vertices.push_back({ c,n,{1,1} });
        vertices.push_back({ a,n,{0,0} });
        vertices.push_back({ c,n,{1,1} });
        vertices.push_back({ d,n,{0,1} });
        };

    // Table dimensions
    float W = 2.0f;   // width (x)
    float Ddim = 1.0f;   // depth (z)
    float H = 0.1f;   // tabletop thickness (y)

    float legW = 0.15f;
    float legH = 1.0f; // leg height downward

    // TABLETOP coordinates
    float yTop = 0.0f;
    float yBottom = -H;

    glm::vec3 A = { -W / 2, yBottom, -Ddim / 2 };
    glm::vec3 B = { W / 2, yBottom, -Ddim / 2 };
    glm::vec3 C = { W / 2, yTop,    -Ddim / 2 };
    glm::vec3 D = { -W / 2, yTop,    -Ddim / 2 };

    glm::vec3 E = { -W / 2, yBottom,  Ddim / 2 };
    glm::vec3 F = { W / 2, yBottom,  Ddim / 2 };
    glm::vec3 G = { W / 2, yTop,     Ddim / 2 };
    glm::vec3 Ht = { -W / 2, yTop,    Ddim / 2 };

    // Top & bottom
    PushQuad(D, C, G, Ht, { 0,1,0 });     // top
    PushQuad(A, B, F, E, { 0,-1,0 });     // bottom

    // Sides
    PushQuad(A, E, Ht, D, { -1,0,0 });
    PushQuad(B, F, G, C, { 1,0,0 });
    PushQuad(A, B, C, D, { 0,0,-1 });
    PushQuad(E, F, G, Ht, { 0,0,1 });

    // ---------------------
    // Legs
    // ---------------------

    auto AddLeg = [&](float x, float z) {
        float x0 = x - legW / 2, x1 = x + legW / 2;
        float z0 = z - legW / 2, z1 = z + legW / 2;

        float y0 = yBottom - legH; // bottom of leg
        float y1 = yBottom;        // attach to tabletop

        glm::vec3 L1 = { x0,y0,z0 };
        glm::vec3 L2 = { x1,y0,z0 };
        glm::vec3 L3 = { x1,y1,z0 };
        glm::vec3 L4 = { x0,y1,z0 };

        glm::vec3 R1 = { x0,y0,z1 };
        glm::vec3 R2 = { x1,y0,z1 };
        glm::vec3 R3 = { x1,y1,z1 };
        glm::vec3 R4 = { x0,y1,z1 };

        PushQuad(L1, L2, L3, L4, { 0,0,-1 });
        PushQuad(R1, R2, R3, R4, { 0,0,1 });
        PushQuad(L1, R1, R4, L4, { -1,0,0 });
        PushQuad(L2, R2, R3, L3, { 1,0,0 });
        PushQuad(L4, L3, R3, R4, { 0,1,0 });
        PushQuad(L1, L2, R2, R1, { 0,-1,0 });
        };

    // 4 legs positioned inside slightly
    float offsetX = W / 2 - legW * 1.2f;
    float offsetZ = Ddim / 2 - legW * 1.2f;

    AddLeg(-offsetX, -offsetZ);
    AddLeg(offsetX, -offsetZ);
    AddLeg(-offsetX, offsetZ);
    AddLeg(offsetX, offsetZ);

    return new Mesh(vertices);
}

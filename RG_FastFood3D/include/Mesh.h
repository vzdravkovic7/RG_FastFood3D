#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct MeshData;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh {
public:
    Mesh();
    Mesh(const std::vector<Vertex>& vertices);
    ~Mesh();

    void Draw() const;

    static Mesh* CreatePattie();
    static Mesh* CreateOven();
    static Mesh* CreateTable();
    static Mesh* CreatePlate();
    static Mesh* CreateSpillQuad();
    static Mesh* CreateSauceBottle();

    void SetTexture(GLuint tex);
    GLuint GetTexture() const;

    void Upload(const MeshData& data);

    glm::vec3 diffuseColor = glm::vec3(1.0f);
    GLuint m_texture = 0;

private:
    unsigned int VAO = 0, VBO = 0;
    size_t vertexCount = 0;
};

#endif

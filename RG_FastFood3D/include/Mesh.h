#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices);

    ~Mesh();

    void Draw() const;

    static Mesh* CreatePattie();
    static Mesh* CreateOven();
    static Mesh* CreateTable();

private:
    unsigned int VAO = 0, VBO = 0;
    size_t vertexCount = 0;
};

#endif

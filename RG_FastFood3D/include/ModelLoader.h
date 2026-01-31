#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Texture.h"
#include <unordered_map>

struct MeshData {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
};

struct MaterialData {
    std::string name;
    glm::vec3 Kd = glm::vec3(1.0f);
    std::string texturePath;
};

class ModelLoader {
public:
    static Mesh* LoadOBJ(const std::string& objPath);
    static std::unordered_map<std::string, MaterialData> LoadMTL(const std::string& mtlPath);
};

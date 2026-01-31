#include "../include/ModelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::unordered_map<std::string, MaterialData> ModelLoader::LoadMTL(const std::string& mtlPath)
{
    std::ifstream file(mtlPath);
    if (!file.is_open()) {
        std::cerr << "[ModelLoader] Could not open MTL file: " << mtlPath << std::endl;
        return {};
    }

    std::unordered_map<std::string, MaterialData> materials;
    MaterialData currentMaterial;

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "newmtl")
        {
            if (!currentMaterial.name.empty())
                materials[currentMaterial.name] = currentMaterial;

            ss >> currentMaterial.name;
            currentMaterial.Kd = glm::vec3(1.0f);
            currentMaterial.texturePath = "";
        }
        else if (type == "Kd")
        {
            ss >> currentMaterial.Kd.r >> currentMaterial.Kd.g >> currentMaterial.Kd.b;
        }
        else if (type == "map_Kd")
        {
            ss >> currentMaterial.texturePath;
        }
    }

    if (!currentMaterial.name.empty())
        materials[currentMaterial.name] = currentMaterial;

    return materials;
}

Mesh* ModelLoader::LoadOBJ(const std::string& objPath)
{
    std::unordered_map<std::string, MaterialData> materials;
    MaterialData* currentMaterial = nullptr;
    std::string objDirectory = objPath.substr(0, objPath.find_last_of("/\\"));

    std::ifstream file(objPath);
    if (!file.is_open()) {
        std::cerr << "[ModelLoader] Could not open OBJ file: " << objPath << std::endl;
        return nullptr;
    }

    // privremeni podaci
    std::vector<glm::vec3> tempPositions;
    std::vector<glm::vec2> tempUVs;
    std::vector<glm::vec3> tempNormals;

    struct FaceIndex { int v = -1, t = -1, n = -1; };

    std::vector<FaceIndex> faces;

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "mtllib")
        {
            std::string mtlName;
            ss >> mtlName;
            std::string fullMtlPath = objDirectory + "/" + mtlName;

            materials = LoadMTL(fullMtlPath);
            continue;
        }
        if (type == "usemtl")
        {
            std::string name;
            ss >> name;

            auto it = materials.find(name);
            if (it != materials.end())
                currentMaterial = &it->second;
            else
                currentMaterial = nullptr;

            continue;
        }

        if (type == "v")
        {
            glm::vec3 v;
            ss >> v.x >> v.y >> v.z;
            tempPositions.push_back(v);
        }
        else if (type == "vt")
        {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            tempUVs.push_back(uv);
        }
        else if (type == "vn")
        {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            tempNormals.push_back(n);
        }
        else if (type == "f")
        {
            // Proèitaj sve tokene u liniji za face
            std::vector<std::string> tokens;
            std::string tok;

            while (ss >> tok)
                tokens.push_back(tok);

            // Ako je quad ? trianguliraj kao (0,1,2) i (0,2,3)
            auto parseIndex = [&](const std::string& fStr) {
                FaceIndex idx;

                int v = 0, t = 0, n = 0;

                // sluèaj: v/t/n
                if (fStr.find('/') != std::string::npos)
                {
                    int slashCount = std::count(fStr.begin(), fStr.end(), '/');

                    if (slashCount == 2)
                    {
                        if (fStr.find("//") != std::string::npos)
                        {
                            // format: v//n
                            sscanf_s(fStr.c_str(), "%d//%d", &v, &n);
                        }
                        else
                        {
                            // format: v/t/n
                            sscanf_s(fStr.c_str(), "%d/%d/%d", &v, &t, &n);
                        }
                    }
                    else if (slashCount == 1)
                    {
                        // format: v/t
                        sscanf_s(fStr.c_str(), "%d/%d", &v, &t);
                    }
                }
                else
                {
                    // samo vertex index
                    sscanf_s(fStr.c_str(), "%d", &v);
                }

                // popravi indeksiranje u 0-based
                idx.v = (v > 0 ? v - 1 : -1);
                idx.t = (t > 0 ? t - 1 : -1);
                idx.n = (n > 0 ? n - 1 : -1);

                return idx;
                };

            if (tokens.size() == 3)
            {
                // veæ trianglovi
                faces.push_back(parseIndex(tokens[0]));
                faces.push_back(parseIndex(tokens[1]));
                faces.push_back(parseIndex(tokens[2]));
            }
            else if (tokens.size() == 4)
            {
                // quad ? dva triangla
                FaceIndex a = parseIndex(tokens[0]);
                FaceIndex b = parseIndex(tokens[1]);
                FaceIndex c = parseIndex(tokens[2]);
                FaceIndex d = parseIndex(tokens[3]);

                faces.push_back(a);
                faces.push_back(b);
                faces.push_back(c);

                faces.push_back(a);
                faces.push_back(c);
                faces.push_back(d);
            }
        }
    }

    MeshData meshData;

    for (auto& f : faces)
    {
        meshData.positions.push_back(tempPositions[f.v]);

        if (f.t >= 0 && f.t < (int)tempUVs.size())
            meshData.uvs.push_back(tempUVs[f.t]);
        else
            meshData.uvs.push_back(glm::vec2(0, 0));

        if (f.n >= 0 && f.n < (int)tempNormals.size())
            meshData.normals.push_back(tempNormals[f.n]);
        else
            meshData.normals.push_back(glm::vec3(0, 1, 0));
    }

    Mesh* mesh = new Mesh();
    mesh->Upload(meshData);

    if (currentMaterial != nullptr)
    {
        mesh->diffuseColor = currentMaterial->Kd;

        if (!currentMaterial->texturePath.empty())
        {
            std::string texPath = objDirectory + "/" + currentMaterial->texturePath;
            mesh->m_texture = Texture::FromFile(texPath.c_str());
        }
    }

    return mesh;
}

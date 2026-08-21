#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

#include <string>
#include <vector>

#include "helper/matrix.h"
#include "core/shader.h"

constexpr int MAX_BONE_INFLUENCE = 4;

struct Vertex
{
    Vec3 Position;
    Vec3 Normal;
    Vec2 TexCoords;
    Vec3 Tangent;
    Vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    uint32_t id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    // mesh Data
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    uint32_t VAO;

    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);

    // render the mesh
    void Draw(Shader &shader);

private:
    // render data
    uint32_t VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};
#endif
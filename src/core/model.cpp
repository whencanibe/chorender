#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core/model.h"

#include "helper/vec.h"
#include "core/mesh.h"
#include "core/shader.h"
#include "core/texture.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>

static uint32_t CreateSolidTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    uint8_t pixel[4] = {r, g, b, a};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return id;
}

static uint32_t GetFallbackTexture(const std::string &typeName)
{
    static uint32_t whiteID = 0;
    static uint32_t normalID = 0;

    if (typeName == "texture_normal")
    {
        if (normalID == 0)
            normalID = CreateSolidTexture(128, 128, 255, 255);
        return normalID;
    }
    if (whiteID == 0)
        // whiteID = CreateSolidTexture(255, 255, 255, 255);
        whiteID = CreateSolidTexture(255, 0, 255, 255); // DEBUG - pink

    return whiteID;
}

Model::Model(std::string const &path, bool gamma) : gammaCorrection(gamma)
{
    loadModel(path);
}

void Model::Draw(Shader &shader)
{
    for (uint32_t i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string const &path)
{   
    // 전역상태 변경: 이미지는 2차원이고 메모리는 1차원이기에 메모리에 이미지 올릴때 원래 기본은 이미지 픽셀의 각행이
    // 4 바이트의 배수인것으로 가정하는데, 아닌 경우에 텍스쳐 비뚤어지거나 하는 경우 발생함.
    // 아래와 같이 쓰면 1바이트 씩 촘촘하게 메모리에 올림
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    // vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        Vec3 vector;

        vector.X = mesh->mVertices[i].x;
        vector.Y = mesh->mVertices[i].y;
        vector.Z = mesh->mVertices[i].z;

        vertex.Position = vector;

        if (mesh->HasNormals())
        {
            vector.X = mesh->mNormals[i].x;
            vector.Y = mesh->mNormals[i].y;
            vector.Z = mesh->mNormals[i].z;

            vertex.Normal = vector;
        }

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            Vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.X = mesh->mTextureCoords[0][i].x;
            vec.Y = mesh->mTextureCoords[0][i].y;

            vertex.TexCoords = vec;
            // tangent
            vector.X = mesh->mTangents[i].x;
            vector.Y = mesh->mTangents[i].y;
            vector.Z = mesh->mTangents[i].z;

            vertex.Tangent = vector;
            // bitangent
            vector.X = mesh->mBitangents[i].x;
            vector.Y = mesh->mBitangents[i].y;
            vector.Z = mesh->mBitangents[i].z;

            vertex.Bitangent = vector;
        }
        else
        {
            vertex.TexCoords = Vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // indices
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // textures
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    Material meshMaterial;

    aiColor3D ks(0.0f, 0.0f, 0.0f);
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, ks) == AI_SUCCESS)
        meshMaterial.Specular = Vec3(ks.r, ks.g, ks.b);

    float ns = 32.0f;
    if (material->Get(AI_MATKEY_SHININESS, ns) == AI_SUCCESS)
        meshMaterial.Shininess = (ns > 1.0f) ? ns : 1.0f;

    std::cout << "diffuse: " << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
    std::cout << "specular: " << material->GetTextureCount(aiTextureType_SPECULAR) << std::endl;
    std::cout << "normal(height): " << material->GetTextureCount(aiTextureType_HEIGHT) << std::endl;
    std::cout << "ambient: " << material->GetTextureCount(aiTextureType_AMBIENT) << std::endl;

    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures, meshMaterial);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {

        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (uint32_t j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), texturePath.C_Str()) == 0)
            {
                Texture cached = textures_loaded[j];
                cached.type = typeName; // GL id는 재사용, 타입은 이번 요청 것으로
                textures.push_back(cached);
                skip = true;
                break;
            }
        }
        if (!skip)
        { // if texture hasn't been loaded already, load it
            // OBJ/MTL files authored on Windows use backslashes; assimp returns them verbatim
            std::string filename = texturePath.C_Str();
            std::replace(filename.begin(), filename.end(), '\\', '/');

            // Only color maps are sRGB; specular/normal/height hold linear data
            const bool srgb = (typeName == "texture_diffuse");

            Texture texture;
            texture.id = LoadTexture2D(this->directory + '/' + filename, srgb);
            if (texture.id == 0) // 로드 실패
                texture.id = GetFallbackTexture(typeName);
            texture.type = typeName;
            texture.path = texturePath.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    if (textures.empty()) // 맵 자체가 없음
    {
        Texture texture;
        texture.id = GetFallbackTexture(typeName);
        texture.type = typeName;
        texture.path = "__fallback_" + typeName;
        textures.push_back(texture);
        // textures_loaded에는 넣지 않습니다
    }
    return textures;
}

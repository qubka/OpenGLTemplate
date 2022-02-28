#pragma once

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "texture.h"

#define INVALID_OGL_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) if (p) { delete p; p = nullptr; }

struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;

    Vertex() {}

    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = normal;
    }
};

class COpenAssetImportMesh
{
public:
    COpenAssetImportMesh();
    ~COpenAssetImportMesh();
    bool Load(const std::filesystem::path& path);
    void Render();

private:
    bool InitFromScene(const aiScene* pScene);
    void InitMesh(uint32_t Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(const std::vector<Vertex>& Vertices,
                  const std::vector<uint32_t>& Indices);
        GLuint vbo;
        GLuint ibo;
        uint32_t NumIndices;
        uint32_t MaterialIndex;
    };

    std::vector<MeshEntry> m_Entries;
    std::vector<CTexture*> m_Textures;
    GLuint m_vao;
    std::filesystem::path m_directory;
};




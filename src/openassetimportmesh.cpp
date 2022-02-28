/*

	Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "openassetimportmesh.h"

COpenAssetImportMesh::MeshEntry::MeshEntry()
{
    vbo = INVALID_OGL_VALUE;
    ibo = INVALID_OGL_VALUE;
    NumIndices  = 0;
    MaterialIndex = INVALID_MATERIAL;
};

COpenAssetImportMesh::MeshEntry::~MeshEntry()
{
    if (vbo != INVALID_OGL_VALUE)
        glDeleteBuffers(1, &vbo);

    if (ibo != INVALID_OGL_VALUE)
        glDeleteBuffers(1, &ibo);
}

void COpenAssetImportMesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                          const std::vector<uint32_t>& Indices)
{
    NumIndices = int(Indices.size());

	glGenBuffers(1, &vbo);
  	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * NumIndices, Indices.data(), GL_STATIC_DRAW);
}

COpenAssetImportMesh::COpenAssetImportMesh()
{
}

COpenAssetImportMesh::~COpenAssetImportMesh()
{
    Clear();
}

void COpenAssetImportMesh::Clear()
{
    for (auto& m_Texture : m_Textures) {
        SAFE_DELETE(m_Texture);
    }
	glDeleteVertexArrays(1, &m_vao);
}

bool COpenAssetImportMesh::Load(const std::filesystem::path& path)
{
    // Release the previously loaded mesh (if it exists)
    Clear();
    
    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    m_directory = path.parent_path();

    if (pScene) {
        Ret = InitFromScene(pScene);
    }
    else {
        std::cerr << "Error loading mesh model: " << Importer.GetErrorString() << std::endl;
    }

    return Ret;
}

bool COpenAssetImportMesh::InitFromScene(const aiScene* pScene)
{  
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

	glGenVertexArrays(1, &m_vao); 
	glBindVertexArray(m_vao);

    // Initialize the meshes in the scene one by one
    for (uint32_t i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene);
}

void COpenAssetImportMesh::InitMesh(uint32_t Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;
    
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (uint32_t i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D& pos      = paiMesh->mVertices[i];
        const aiVector3D& normal   = paiMesh->mNormals[i];
        const aiVector3D& texCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;

        Vertices.emplace_back(
                glm::vec3{pos.x, pos.y, pos.z},
                glm::vec2{texCoord.x, 1.0f-texCoord.y},
                glm::vec3{normal.x, normal.y, normal.z}
        );
    }

    for (uint32_t i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool COpenAssetImportMesh::InitMaterials(const aiScene* pScene)
{
    bool Ret = true;

    // Initialize the materials
    for (uint32_t i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = nullptr;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString str;

            // TODO: Load all texture, not only the first one
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS) {
                std::filesystem::path path = m_directory;
                path /= str.C_Str();

                m_Textures[i] = new CTexture();
                if (!m_Textures[i]->Load(path, true)) {
                    std::cerr << "Error loading mesh texture: " << path << std::endl;

                    delete m_Textures[i];
                    m_Textures[i] = nullptr;
                    Ret = false;
                }
                else {
                    std::cout << "Loaded texture: " << path << std::endl;
                }
            }
        }

        // Load a single colour texture matching the diffuse colour if no texture added
        if (!m_Textures[i]) {
		
			aiColor3D color (0.f,0.f,0.f);
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE,color);

			m_Textures[i] = new CTexture();
			uint8_t data[3];
			data[0] = (uint8_t) (color[2]*255);
			data[1] = (uint8_t) (color[1]*255);
			data[2] = (uint8_t) (color[0]*255);
			m_Textures[i]->CreateFromData(data, 1, 1, 24, GL_BGR, false);
        }
    }

    return Ret;
}

void COpenAssetImportMesh::Render()
{
	glBindVertexArray(m_vao);

    for (auto& entry : m_Entries) {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, entry.vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_tex));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_normal));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entry.ibo);

        const uint32_t MaterialIndex = entry.MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(0);
        }

        glDrawElements(GL_TRIANGLES, entry.NumIndices, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
    }
}

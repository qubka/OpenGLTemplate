#pragma once

class CVertexBufferObjectIndexed
{
public:
	CVertexBufferObjectIndexed();
	~CVertexBufferObjectIndexed();

	void Create();									// Creates a VBO
	void Bind();									// Binds the VBO
	void Release();									// Releases the VBO

	void AddVertexData(void* pVertexData, GLuint vertexDataSize);	// Adds vertex data
	void AddIndexData(void* pIndexData, GLuint indexDataSize);	// Adds index data
	void UploadDataToGPU(int iUsageHint);			// Upload the VBO to the GPU


private:
	GLuint m_vboVertices;		// VBO id for vertices
	GLuint m_vboIndices;		// VBO id for indices

	std::vector<uint8_t> m_vertexData;	// Vertex data to be uploaded
    std::vector<uint8_t> m_indexData;	// Index data to be uploaded

	bool m_dataUploaded;		// Flag indicating if data is uploaded to the GPU
};
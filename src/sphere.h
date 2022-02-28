#pragma once

#include "texture.h"
#include "vertexbufferobjectindexed.h"

// Class for generating a unit sphere
class CSphere
{
public:
	CSphere();
	~CSphere();
	void Create(const std::string& directory, const std::string& front, int slicesIn, int stacksIn);
	void Render();
	void Release();

private:
	GLuint m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	std::string m_directory;
	std::string m_filename;
	int m_numTriangles;
};
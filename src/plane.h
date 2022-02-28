#pragma once

#include "texture.h"
#include "vertexbufferobject.h"

// Class for generating a xz plane of a given size
class CPlane
{
public:
	CPlane();
	~CPlane();
	void Create(const std::string& sDirectory, const std::string& sFilename, float fWidth, float fHeight, float fTextureRepeat);
	void Render();
	void Release();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	std::string m_directory;
	std::string m_filename;
	float m_width;
	float m_height;
};
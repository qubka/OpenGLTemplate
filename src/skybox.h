#pragma once

#include "texture.h"
#include "vertexbufferobject.h"
#include "cubemap.h"

// This is a class for creating and rendering a skybox
class CSkybox
{
public:
	CSkybox();
	~CSkybox();
	void Create(float size);
	void Render(int textureUnit);
	void Release();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CCubemap m_cubemapTexture;
};
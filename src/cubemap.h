#pragma once

#include "texture.h"
#include "vertexbufferobject.h"

class CCubemap
{
public:
	void Create(const std::string& sPositiveX, const std::string& sNegativeX, const std::string& sPositiveY, const std::string& sNegativeY, const std::string& sPositiveZ, const std::string& sNegativeZ);
	void Release();
	void Bind(int iTextureUnit = 0);

private:
	GLuint m_uiVAO;
	CVertexBufferObject m_vboRenderData;
	GLuint m_uiTexture;
	GLuint m_uiSampler; // Sampler name
};
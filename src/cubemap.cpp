#include "cubemap.h"
#include "image.h"

// Binds a texture for rendering
void CCubemap::Bind(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0+iTextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uiTexture);
	glBindSampler(iTextureUnit, m_uiSampler);
}

// Create the plane, including its geometry, texture mapping, normal, and colour
void CCubemap::Create(const std::string& sPositiveX, const std::string& sNegativeX, const std::string& sPositiveY, const std::string& sNegativeY, const std::string& sPositiveZ, const std::string& sNegativeZ)
{
	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uiTexture);

	Image positiveX {sPositiveX};
	Image negativeX {sNegativeX};
	Image positiveY {sPositiveY};
	Image negativeY {sNegativeY};
	Image positiveZ {sPositiveZ};
	Image negativeZ {sNegativeZ};

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB8, positiveX.width, positiveX.height, 0, GL_BGR, GL_UNSIGNED_BYTE, positiveX.pixels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB8, negativeX.width, negativeX.height, 0, GL_BGR, GL_UNSIGNED_BYTE, negativeX.pixels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB8, positiveY.width, positiveY.height, 0, GL_BGR, GL_UNSIGNED_BYTE, positiveY.pixels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB8, negativeY.width, negativeY.height, 0, GL_BGR, GL_UNSIGNED_BYTE, negativeY.pixels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB8, positiveZ.width, positiveZ.height, 0, GL_BGR, GL_UNSIGNED_BYTE, positiveZ.pixels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB8, negativeZ.width, negativeZ.height, 0, GL_BGR, GL_UNSIGNED_BYTE, negativeZ.pixels);

	glGenSamplers(1, &m_uiSampler);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

// Release resources
void CCubemap::Release()
{
	glDeleteSamplers(1, &m_uiSampler);
	glDeleteTextures(1, &m_uiTexture);
}
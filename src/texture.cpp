#include "texture.h"
#include "image.h"

CTexture::CTexture()
{
	m_mipMapsGenerated = false;
}
CTexture::~CTexture()
{}

// Create a texture from the data stored in bData.  
void CTexture::CreateFromData(uint8_t* data, int width, int height, int channels, GLenum internalFormat, GLenum dataFormat, bool generateMipMaps)
{
	// Generate an OpenGL texture ID for this texture
    glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

	if(generateMipMaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
	glGenSamplers(1, &m_samplerObjectID);

	m_path = "";
	m_mipMapsGenerated = generateMipMaps;
	m_width = width;
	m_height = height;
	m_channels = channels;
}

// Loads a 2D texture given the filename (sPath).  bGenerateMipMaps will generate a mipmapped texture if true
bool CTexture::Load(const std::string& path, bool generateMipMaps)
{
	Image image {path};

	// If somehow one of these failed (they shouldn't), return failure
	if (image.pixels == nullptr || image.height == 0 || image.width == 0)
		return false;

    GLenum internalFormat = GL_R8, dataFormat = GL_RED;
    switch (image.channels) {
        case 3:
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
            break;
        case 4:
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
            break;
    }

	CreateFromData(image.pixels, image.width, image.height, image.channels, internalFormat, dataFormat, generateMipMaps);

	m_path = path;

	return true; // Success
}

void CTexture::SetSamplerObjectParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(m_samplerObjectID, parameter, value);
}

void CTexture::SetSamplerObjectParameterf(GLenum parameter, float value)
{
	glSamplerParameterf(m_samplerObjectID, parameter, value);
}

// Binds a texture for rendering
void CTexture::Bind(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0+iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glBindSampler(iTextureUnit, m_samplerObjectID);
}

// Frees memory on the GPU of the texture
void CTexture::Release()
{
	glDeleteSamplers(1, &m_samplerObjectID);
	glDeleteTextures(1, &m_textureID);
}

int CTexture::GetWidth()
{
	return m_width;
}

int CTexture::GetHeight()
{
	return m_height;
}

int CTexture::GetChannels()
{
	return m_channels;
}
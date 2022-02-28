#pragma once

// Class that provides a texture for texture mapping in OpenGL
class CTexture
{
public:
    CTexture();
    ~CTexture();

	void CreateFromData(uint8_t* data, int width, int height, int channels, GLenum internalFormat, GLenum dataFormat, bool generateMipMaps = false);
	bool Load(const std::string& path, bool generateMipMaps = true);
	void Bind(int textureUnit = 0);

	void SetSamplerObjectParameter(GLenum parameter, GLenum value);
	void SetSamplerObjectParameterf(GLenum parameter, float value);

	int GetWidth();
	int GetHeight();
	int GetChannels();

	void Release();

private:
	int m_width, m_height, m_channels; // Texture width, height, and channels
	GLuint m_textureID; // Texture id
	GLuint m_samplerObjectID; // Sampler id
	bool m_mipMapsGenerated;

	std::string m_path;
};


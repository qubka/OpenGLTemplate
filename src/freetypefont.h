#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "texture.h"
#include "shaders.h"
#include "vertexbufferobject.h"

// This class is a wrapper for FreeType fonts and their usage with OpenGL
class CFreeTypeFont
{
public:
	CFreeTypeFont();
	~CFreeTypeFont();

	bool LoadFont(const std::string& file, int pixelSize);

	int GetTextWidth(const std::string& text, int pixelSize);

	void Print(const std::string& text, int x, int y, int pixelSize = -1);
	void Render(int x, int y, int pixelSize, const char* text, ...);

	void ReleaseFont();

	void SetShaderProgram(CShaderProgram* shaderProgram);

private:
	void CreateChar(int index);

    CTexture m_charTextures[UCHAR_MAX+1];
	int m_advX[UCHAR_MAX+1], m_advY[UCHAR_MAX+1];
	int m_bearingX[UCHAR_MAX+1], m_bearingY[UCHAR_MAX+1];
	int m_charWidth[UCHAR_MAX+1], m_charHeight[UCHAR_MAX+1];
	int m_loadedPixelSize, m_newLine;

	bool m_isLoaded;

	GLuint m_vao;
	CVertexBufferObject m_vbo;

	FT_Library m_ftLib;
	FT_Face m_ftFace;
	CShaderProgram* m_shaderProgram;
};

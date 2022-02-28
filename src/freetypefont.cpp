#include "freetypefont.h"

CFreeTypeFont::CFreeTypeFont()
{
	m_isLoaded = false;
}
CFreeTypeFont::~CFreeTypeFont()
{}

/*-----------------------------------------------

Name:	createChar

Params:	iIndex - character index in Unicode.

Result:	Creates one single character (its
		texture).

/*---------------------------------------------*/

inline int next_p2(int n){int res = 1; while(res < n)res <<= 1; return res;}

void CFreeTypeFont::CreateChar(int index)
{
	FT_Load_Glyph(m_ftFace, FT_Get_Char_Index(m_ftFace, index), FT_LOAD_DEFAULT);

    FT_GlyphSlot& glyph = m_ftFace->glyph;
    FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap* pBitmap = &glyph->bitmap;

	int iW = pBitmap->width, iH = pBitmap->rows;
	int iTW = next_p2(iW), iTH = next_p2(iH);

	std::vector<uint8_t> pixels(iTW * iTH);
	// Copy glyph data and add dark pixels elsewhere
	for (int ch = 0; ch < iTH; ch++) 
		for (int cw = 0; cw < iTW; cw++)
            pixels[ch * iTW + cw] = (ch >= iH || cw >= iW) ? 0 : pBitmap->buffer[(iH - ch - 1) * iW + cw];
 
	// And create a texture from it

	m_charTextures[index].CreateFromData(pixels.data(), iTW, iTH, 1, GL_R8, GL_RED, false);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_charTextures[index].SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Calculate glyph data
	m_advX[index] = glyph->advance.x >> 6;
	m_bearingX[index] = glyph->metrics.horiBearingX >> 6;
	m_charWidth[index] = glyph->metrics.width >> 6;

	m_advY[index] = (glyph->metrics.height - glyph->metrics.horiBearingY) >> 6;
	m_bearingY[index] = glyph->metrics.horiBearingY >> 6;
	m_charHeight[index] = glyph->metrics.height >> 6;

	m_newLine = std::max(m_newLine, int(glyph->metrics.height >> 6));

	// Rendering data, texture coordinates are always the same, so now we waste a little memory
	glm::vec2 vQuad[] = {
        {0.0f, float(-m_advY[index]+iTH)},
        {0.0f, float(-m_advY[index])},
        {float(iTW), float(-m_advY[index]+iTH)},
        {float(iTW), float(-m_advY[index])}
	};

	glm::vec2 vTexQuad[] = {
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 1.0f},
            {1.0f, 0.0f}
    };

	// Add this char to VBO
	for (int i = 0; i < 4; i++) {
		m_vbo.AddData(&vQuad[i], sizeof(glm::vec2));
		m_vbo.AddData(&vTexQuad[i], sizeof(glm::vec2));
	}
}

// Loads an entire font with the given path sFile and pixel size iPXSize
bool CFreeTypeFont::LoadFont(const std::string& file, int ipixelSize)
{
    bool bError = FT_Init_FreeType(&m_ftLib);
    assert(!bError && "Failed to initialize FreeType!");

	bError = FT_New_Face(m_ftLib, file.c_str(), 0, &m_ftFace);
	if(bError) {
        std::cerr << "Cannot load font: " << file << std::endl;
		return false;
	}

	FT_Set_Pixel_Sizes(m_ftFace, ipixelSize, ipixelSize);
	m_loadedPixelSize = ipixelSize;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	for (int i = 0; i < 128; i++)
		CreateChar(i);
	m_isLoaded = true;

	FT_Done_Face(m_ftFace);
	FT_Done_FreeType(m_ftLib);
	
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, (void*)(sizeof(glm::vec2)));

	return true;
}

// Prints text at the specified location (x, y) with the given pixel size (iPXSize)
void CFreeTypeFont::Print(const std::string& text, int x, int y, int pixelSize)
{
	if(!m_isLoaded)
		return;

	glBindVertexArray(m_vao);
	m_shaderProgram->SetUniform("sampler0", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int iCurX = x, iCurY = y;
	if (pixelSize == -1)
		pixelSize = m_loadedPixelSize;
	float fScale = float(pixelSize) / float(m_loadedPixelSize);
	for (unsigned char i : text) {
        if (i == '\0')
            break;

		if (i == '\n') {
			iCurX = x;
			iCurY -= m_newLine*pixelSize / m_loadedPixelSize;
			continue;
		}
		iCurX += m_bearingX[i] * pixelSize / m_loadedPixelSize;
		if(i != ' ') {
			m_charTextures[i].Bind();
			glm::mat4 mModelView = glm::translate(glm::mat4{ 1 }, glm::vec3{float(iCurX), float(iCurY), 0.0f});
			mModelView = glm::scale(mModelView, glm::vec3{fScale});
			m_shaderProgram->SetUniform("matrices.modelViewMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
		}

		iCurX += (m_advX[i] - m_bearingX[i])*pixelSize / m_loadedPixelSize;
	}
	glDisable(GL_BLEND);
}


// Print formatted text at the location (x, y) with specified pixel size (iPXSize)
void CFreeTypeFont::Render(int x, int y, int pixelSize, const char* text, ...)
{
    va_list args;
    va_start(args, text);
    char buf[32];
    size_t n = std::vsnprintf(buf, sizeof(buf), text, args);
    va_end(args);

    // Static buffer large enough?
    if (n < sizeof(buf)) {
        Print({buf, n}, x, y, pixelSize);
        return;
    }

    // Static buffer too small
    std::string s(n + 1, 0);
    va_start(args, text);
    std::vsnprintf(const_cast<char*>(s.data()), s.size(), text, args);
    va_end(args);

    Print(s, x, y, pixelSize);
}

// Deletes all font textures
void CFreeTypeFont::ReleaseFont()
{
	for (int i = 0; i <= CHAR_MAX; i++)
		m_charTextures[i].Release();
	m_vbo.Release();
	glDeleteVertexArrays(1, &m_vao);
}

// Gets the width of text
int CFreeTypeFont::GetTextWidth(const std::string& sText, int iPixelSize)
{
	int iResult = 0;
	for (char i : sText)
		iResult += m_advX[i];
	return iResult * iPixelSize / m_loadedPixelSize;
}

// Sets shader programme that font uses
void CFreeTypeFont::SetShaderProgram(CShaderProgram* shaderProgram)
{
	m_shaderProgram = shaderProgram;
}
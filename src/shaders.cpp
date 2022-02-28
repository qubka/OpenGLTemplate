#include "shaders.h"

CShader::CShader()
{
	m_bLoaded = false;
}

CShader::~CShader()
{}

// Loads a shader, stored as a text file with filename sFile.  The shader is of type iType (vertex, fragment, geometry, etc.)
bool CShader::LoadShader(const std::string& sFile, int iType)
{
    m_uiShader = glCreateShader(iType);

    std::string sShaderCode = ReadFile(sFile);
    const GLchar* code = sShaderCode.c_str();
    glShaderSource(m_uiShader, 1, &code, nullptr);
	glCompileShader(m_uiShader);

    GLint iCompilationStatus;
	glGetShaderiv(m_uiShader, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)
	{
        int iLogLength;
        char sInfoLog[1024];
		glGetShaderInfoLog(m_uiShader, 1024, &iLogLength, sInfoLog);
		std::string sShaderType;
		if (iType == GL_VERTEX_SHADER)
            sShaderType = "vertex shader";
		else if (iType == GL_FRAGMENT_SHADER)
            sShaderType = "fragment shader";
		else if (iType == GL_GEOMETRY_SHADER)
            sShaderType = "geometry shader";
		else if (iType == GL_TESS_CONTROL_SHADER)
            sShaderType = "tesselation control shader";
		else if (iType == GL_TESS_EVALUATION_SHADER)
            sShaderType = "tesselation evaluation shader";
		else
            sShaderType = "unknown shader type";

		std::cout << "Error in " << sShaderType << '\n' << sFile << '\n' << "Shader file not compiled. The compiler returned:\n\n" << sInfoLog << std::endl;
		return false;
	}
	m_iType = iType;
	m_bLoaded = true;

	return true;
}

// Loads a file into a vector of std::strings (vResult)
std::string CShader::ReadFile(const std::string& path)
{
    std::ifstream file(path);
    if (file) {
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        return stream.str();
    } else {
        std::cerr << "Can't open file: " << path << std::endl;
        std::abort();
    }
}

// Returns true if the shader was loaded and compiled
bool CShader::IsLoaded()
{
	return m_bLoaded;
}

// Returns the ID of the shader
GLuint CShader::GetShaderID()
{
	return m_uiShader;
}

// Deletes the shader and frees GPU memory
void CShader::DeleteShader()
{
	if(!IsLoaded())
		return;
	m_bLoaded = false;
	glDeleteShader(m_uiShader);
}

CShaderProgram::CShaderProgram()
{
	m_bLinked = false;
}

// Creates a new shader program
void CShaderProgram::CreateProgram()
{
	m_uiProgram = glCreateProgram();
}

// Adds a compiled shader to a program
bool CShaderProgram::AddShaderToProgram(CShader* shShader)
{
	if(!shShader->IsLoaded())
		return false;

	glAttachShader(m_uiProgram, shShader->GetShaderID());

	return true;
}

// Performs final linkage of the OpenGL shader program
bool CShaderProgram::LinkProgram()
{
	glLinkProgram(m_uiProgram);
	int iLinkStatus;
	glGetProgramiv(m_uiProgram, GL_LINK_STATUS, &iLinkStatus);

	if (iLinkStatus == GL_FALSE) 
	{
        int iLogLength;
        char sInfoLog[1024];
		glGetProgramInfoLog(m_uiProgram, 1024, &iLogLength, sInfoLog);
		std::cerr << "Error! Shader program wasn't linked! The linker returned: " << sInfoLog << std::endl;
		return false;
	}

	m_bLinked = iLinkStatus == GL_TRUE;
	return m_bLinked;
}

// Deletes the program and frees memory on the GPU
void CShaderProgram::DeleteProgram()
{
	if(!m_bLinked)
		return;
	m_bLinked = false;
	glDeleteProgram(m_uiProgram);
}

// Instructs OpenGL to use this program
void CShaderProgram::UseProgram()
{
	if(m_bLinked)
		glUseProgram(m_uiProgram);
}

// Returns the OpenGL program ID
GLuint CShaderProgram::GetProgramID()
{
	return m_uiProgram;
}

// A collection of functions to set uniform variables inside shaders

// Setting floats

void CShaderProgram::SetUniform(const std::string& sName, float* fValues, int iCount) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1fv(iLoc, iCount, fValues);
}

void CShaderProgram::SetUniform(const std::string& sName, const float fValue) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1fv(iLoc, 1, &fValue);
}

// Setting vectors

void CShaderProgram::SetUniform(const std::string& sName, glm::vec2* vVectors, int iCount) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform2fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(const std::string& sName, const glm::vec2& vVector) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
}

void CShaderProgram::SetUniform(const std::string& sName, glm::vec3* vVectors, int iCount) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(const std::string& sName, const glm::vec3& vVector) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}

void CShaderProgram::SetUniform(const std::string& sName, glm::vec4* vVectors, int iCount) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform4fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(const std::string& sName, const glm::vec4& vVector) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform4fv(iLoc, 1, (GLfloat*)&vVector);
}

// Setting 3x3 matrices

void CShaderProgram::SetUniform(const std::string& sName, glm::mat3* mMatrices, int iCount) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix3fv(iLoc, iCount, GL_FALSE, (GLfloat*)mMatrices);
}

void CShaderProgram::SetUniform(const std::string& sName, const glm::mat3& mMatrix) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix3fv(iLoc, 1, GL_FALSE, (GLfloat*)&mMatrix);
}

// Setting 4x4 matrices

void CShaderProgram::SetUniform(const std::string& sName, glm::mat4* mMatrices, int iCount) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix4fv(iLoc, iCount, GL_FALSE, (GLfloat*)mMatrices);
}

void CShaderProgram::SetUniform(const std::string& sName, const glm::mat4& mMatrix) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix4fv(iLoc, 1, GL_FALSE, (GLfloat*)&mMatrix);
}

// Setting integers

void CShaderProgram::SetUniform(const std::string& sName, int* iValues, int iCount) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1iv(iLoc, iCount, iValues);
}

void CShaderProgram::SetUniform(const std::string& sName, const int iValue) const
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1i(iLoc, iValue);
}
#pragma once

// A class that provides a wrapper around an OpenGL shader
class CShader
{
public:
	CShader();
	~CShader();

	bool LoadShader(const std::string& sFile, int iType);
	void DeleteShader();

	bool IsLoaded();
	GLuint GetShaderID();


private:
	GLuint m_uiShader; // ID of shader
	int m_iType; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool m_bLoaded; // Whether shader was loaded and compiled

    static std::string ReadFile(const std::string& path);
};

// A class the provides a wrapper around an OpenGL shader program
class CShaderProgram
{
public:
	CShaderProgram();

	void CreateProgram();
	void DeleteProgram();

	bool AddShaderToProgram(CShader* shShader);
	bool LinkProgram();

	void UseProgram();

	GLuint GetProgramID();

	// Setting vectors
	void SetUniform(const std::string& sName, glm::vec2* vVectors, int iCount = 1) const;
	void SetUniform(const std::string& sName, const glm::vec2& vVector) const;
	void SetUniform(const std::string& sName, glm::vec3* vVectors, int iCount = 1) const;
	void SetUniform(const std::string& sName, const glm::vec3& vVector) const;
	void SetUniform(const std::string& sName, glm::vec4* vVectors, int iCount = 1) const;
	void SetUniform(const std::string& sName, const glm::vec4& vVector) const;

	// Setting floats
	void SetUniform(const std::string& sName, float* fValues, int iCount = 1) const;
	void SetUniform(const std::string& sName, const float fValue) const;

	// Setting 3x3 matrices
	void SetUniform(const std::string& sName, glm::mat3* mMatrices, int iCount = 1) const;
	void SetUniform(const std::string& sName, const glm::mat3& mMatrix) const;

	// Setting 4x4 matrices
	void SetUniform(const std::string& sName, glm::mat4* mMatrices, int iCount = 1) const;
	void SetUniform(const std::string& sName, const glm::mat4& mMatrix) const;

	// Setting integers
	void SetUniform(const std::string& sName, int* iValues, int iCount = 1) const;
	void SetUniform(const std::string& sName, const int iValue) const;


private:
	GLuint m_uiProgram; // ID of program
	bool m_bLinked; // Whether program was linked and is ready to use
};

#define _USE_MATH_DEFINES
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

#include "sphere.h"

CSphere::CSphere()
{}

CSphere::~CSphere()
{}

// Create a unit sphere 
void CSphere::Create(const std::string& a_sDirectory, const std::string& a_sFilename, int slicesIn, int stacksIn)
{
	// check if filename passed in -- if so, load texture

	m_texture.Load(a_sDirectory+a_sFilename);

	m_directory = a_sDirectory;
	m_filename = a_sFilename;

	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	// Compute vertex attributes and store in VBO
	int vertexCount = 0;
	for (int stacks = 0; stacks < stacksIn; stacks++) {
		float phi = (stacks / (float) (stacksIn - 1)) * (float) M_PI;
		for (int slices = 0; slices <= slicesIn; slices++) {
			float theta = (slices / (float) slicesIn) * 2 * (float) M_PI;
			
			glm::vec3 v = glm::vec3{cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)};
			glm::vec2 t = glm::vec2{slices / (float) slicesIn, stacks / (float) stacksIn};
			glm::vec3 n = v;

			m_vbo.AddVertexData(&v, sizeof(glm::vec3));
			m_vbo.AddVertexData(&t, sizeof(glm::vec2));
			m_vbo.AddVertexData(&n, sizeof(glm::vec3));

			vertexCount++;

		}
	}

	// Compute indices and store in VBO
	m_numTriangles = 0;
	for (int stacks = 0; stacks < stacksIn; stacks++) {
		for (int slices = 0; slices < slicesIn; slices++) {
			uint32_t nextSlice = slices + 1;
			uint32_t nextStack = (stacks + 1) % stacksIn;

			uint32_t index0 = stacks * (slicesIn+1) + slices;
			uint32_t index1 = nextStack * (slicesIn+1) + slices;
			uint32_t index2 = stacks * (slicesIn+1) + nextSlice;
			uint32_t index3 = nextStack * (slicesIn+1) + nextSlice;

			m_vbo.AddIndexData(&index0, sizeof(uint32_t));
			m_vbo.AddIndexData(&index1, sizeof(uint32_t));
			m_vbo.AddIndexData(&index2, sizeof(uint32_t));
			m_numTriangles++;

			m_vbo.AddIndexData(&index2, sizeof(uint32_t));
			m_vbo.AddIndexData(&index1, sizeof(uint32_t));
			m_vbo.AddIndexData(&index3, sizeof(uint32_t));
			m_numTriangles++;

		}
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
	
}

// Render the sphere as a set of triangles
void CSphere::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawElements(GL_TRIANGLES, m_numTriangles*3, GL_UNSIGNED_INT, 0);
}

// Release memory on the GPU 
void CSphere::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}
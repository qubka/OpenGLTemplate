/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/


#include "game.h"


// Setup includes
#include "window.h"
#include "input.h"

// Game includes
#include "camera.h"
#include "skybox.h"
#include "plane.h"
#include "shaders.h"
#include "freetypefont.h"
#include "sphere.h"
#include "matrixstack.h"
#include "openassetimportmesh.h"
#include "audiomanager.h"

// Constructor
Game::Game() : m_window {"OpenGL Template", {1280, 720}}
{
    Input::Init(m_window);

	m_pSkybox = nullptr;
	m_pCamera = nullptr;
	m_pShaderPrograms = nullptr;
	m_pPlanarTerrain = nullptr;
	m_pFtFont = nullptr;
	m_pBarrelMesh = nullptr;
	m_pHorseMesh = nullptr;
	m_pSphere = nullptr;
    m_pAudioManager = nullptr;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pSphere;

    m_pAudioManager->Destroy();
	delete m_pAudioManager;

	if (m_pShaderPrograms != nullptr) {
		for (auto & m_pShaderProgram : *m_pShaderPrograms)
			delete m_pShaderProgram;
	}
	delete m_pShaderPrograms;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
    // Set the clear colour and depth
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearDepth(1.0f);

    /// Create objects
    m_pCamera = new CCamera;
    m_pSkybox = new CSkybox;
    m_pShaderPrograms = new std::vector<CShaderProgram*>;
    m_pPlanarTerrain = new CPlane;
    m_pFtFont = new CFreeTypeFont;
    m_pBarrelMesh = new COpenAssetImportMesh;
    m_pHorseMesh = new COpenAssetImportMesh;
    m_pSphere = new CSphere;
    m_pAudioManager = new CAudioManager;

    // Set the orthographic and perspective projection matrices based on the image size
    m_pCamera->SetOrthographicProjectionMatrix(m_window.GetWidth(), m_window.GetHeight());
    m_pCamera->SetPerspectiveProjectionMatrix(45.0f, m_window.GetAspect(), 0.5f, 5000.0f);

    // Load shaders
    std::vector<CShader> shShaders;
    std::vector<std::string> sShaderFileNames;
    sShaderFileNames.emplace_back("mainShader.vert");
    sShaderFileNames.emplace_back("mainShader.frag");
    sShaderFileNames.emplace_back("textShader.vert");
    sShaderFileNames.emplace_back("textShader.frag");

    for (auto& sShaderFileName: sShaderFileNames) {
        std::string sExt = sShaderFileName.substr((int) sShaderFileName.size() - 4, 4);
        int iShaderType;
        if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
        else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
        else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
        else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
        else iShaderType = GL_TESS_EVALUATION_SHADER;
        CShader shader;
        shader.LoadShader("resources/shaders/" + sShaderFileName, iShaderType);
        shShaders.push_back(shader);
    }

    // Create the main shader program
    auto* pMainProgram = new CShaderProgram;
    pMainProgram->CreateProgram();
    pMainProgram->AddShaderToProgram(&shShaders[0]);
    pMainProgram->AddShaderToProgram(&shShaders[1]);
    pMainProgram->LinkProgram();
    m_pShaderPrograms->push_back(pMainProgram);

    // Create a shader program for fonts
    auto* pFontProgram = new CShaderProgram;
    pFontProgram->CreateProgram();
    pFontProgram->AddShaderToProgram(&shShaders[2]);
    pFontProgram->AddShaderToProgram(&shShaders[3]);
    pFontProgram->LinkProgram();
    m_pShaderPrograms->push_back(pFontProgram);

    // You can follow this pattern to load additional shaders

    // Create the skybox
    // Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
    m_pSkybox->Create(2500.0f);

    // Create the planar terrain
    m_pPlanarTerrain->Create("resources/textures/", "grassfloor01.jpg", 2000.0f, 2000.0f,
                             50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

    m_pFtFont->LoadFont("resources/fonts/arial.ttf", 32);
    m_pFtFont->SetShaderProgram(pFontProgram);

    // Load some meshes in OBJ format
    m_pBarrelMesh->Load("resources/models/Barrel/barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
    m_pHorseMesh->Load("resources/models/Horse/horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013

    // Create a sphere
    m_pSphere->Create("resources/textures/", "dirtpile01.jpg", 25,
                      25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
    glEnable(GL_CULL_FACE);

    // Initialise audio and play background music
    m_pAudioManager->Initialise();
    m_pAudioManager->Load("resources/audio/Boing.wav");                    // Royalty free sound from freesound.org
    m_pAudioManager->Load("resources/audio/fsm-team-escp-paradox.wav");    // Royalty free sound from freesound.org
    m_pAudioManager->Play("resources/audio/Boing.wav", m_pCamera->GetPosition());
}

// Render method runs repeatedly in a loop
void Game::Render()
{
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4{-100, 100, -100, 1}; // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3{1.0f});		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3{1.0f});		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3{1.0f});		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3{1.0f});	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3{0.0f});	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3{0.0f});	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
		

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3{0.5f});	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3{0.5f});	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3{1.0f});	// Specular material reflectance


	// Render the horse 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3{0.0f, 0.0f, 0.0f});
		modelViewMatrixStack.Rotate(glm::vec3{0.0f, 1.0f, 0.0f}, 180.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the barrel 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3{100.0f, 0.0f, 0.0f});
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the sphere
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3{0.0f, 2.0f, 150.0f});
		modelViewMatrixStack.Scale(2.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		m_pSphere->Render();
	modelViewMatrixStack.Pop();

    CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

    // Use the font shader program and render the text
    fontProgram->UseProgram();
    glDisable(GL_DEPTH_TEST);
    fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4{ 1 });
    fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
    fontProgram->SetUniform("vColour", glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});

    m_pFtFont->Render(20, 20, 20, "Press TAB to lock mouse and use camera");
    m_pFtFont->Render(20, 50, 20, "Press ESC to exit");
    m_pFtFont->Render(20, 80, 20, "Press F1 to enable wiremode renderer");

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();
}

// Update method runs repeatedly with the Render method
void Game::Update()
{
    if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
        m_window.ShouldClose(true);
    }

    if (Input::GetKeyDown(GLFW_KEY_TAB)) {
        m_window.ToggleCursor();
    }

    if (Input::GetKeyDown(GLFW_KEY_F1)) {
        m_window.ToggleWireframe();
    }

    m_pAudioManager->Play("resources/audio/fsm-team-escp-paradox.wav", m_pCamera->GetPosition());

	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_pCamera->Update(m_dt);

	m_pAudioManager->Update();
}

void Game::DisplayFrameRate()
{
	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1.0f)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		m_pFtFont->Render(20, m_window.GetHeight() - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

// The game loop runs repeatedly until game over
void Game::Run()
{
    float currentTime = static_cast<float>(glfwGetTime());
    float previousTime = currentTime;

    while (!m_window.ShouldClose()) {
        currentTime = static_cast<float>(glfwGetTime());
        m_dt = currentTime - previousTime;
        previousTime = currentTime;

        Update();
        Render();

        // Reset input data
        Input::Update();

        // Swap buffers to show the rendered image
        m_window.SwapBuffers();
        m_window.PollEvents();
    }
}

Game& Game::GetInstance() 
{
	static Game instance;
	return instance;
}

int main(int args, char** argv) {
    Game& game = Game::GetInstance();
    try {
        game.Initialise();
        game.Run();
    } catch(const std::system_error& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
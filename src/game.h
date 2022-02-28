#pragma once

#include "window.h"

int main(int argc, char** argv);

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to nullptr and in Game::Initialise, create a new object.  Don't forget to
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CSphere;
class COpenAssetImportMesh;
class CAudio;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	std::vector<CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	CSphere *m_pSphere;
	CAudio *m_pAudio;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;

public:
	Game();
	~Game();
	static Game& GetInstance();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void Run();
	Window m_window;
	int m_frameCount;
	double m_elapsedTime;

    friend int ::main(int argc, char** argv);
};

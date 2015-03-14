// Graphics.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html
// Edited by Blake Zoeckler

#pragma once
#include <windows.h>

#include "d3dclass.h"
#include "cameraclass.h"
#include "textclass.h"
#include "particleshaderclass.h"
#include "particlesystemclass.h"
#include "EntityManager.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(int screenWidth, int screenHeight, HWND);
	void Shutdown();
	bool Frame(int fps, int cpu, float frameTime);

	D3DXVECTOR3 GetCameraPosition();
	void SetCameraPosition(float, float);
	void TranslateCamera(float, float);
	void SetCameraShift(float, float);

	EntityManager* GetEntityManager();

	void SetCursorPosition(float x, float y);
	void GetCursorPosition(float& x, float& y);

private:
	bool Render();
	void InitializeEntities();

private:
	D3DClass* m_D3D;

	CameraClass* m_Camera;
	CameraClass* m_GUICamera;
	TextClass* m_Text;
	TextureShaderClass* m_TextureShader;
	EntityManager* m_EntityManager;
	Entity* m_Cursor;

	ParticleShaderClass* m_ParticleShader;
};


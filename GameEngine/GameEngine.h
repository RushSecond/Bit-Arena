// GameEngine.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html
// Edited by Blake Zoeckler

#pragma once

#include "resource.h"
#include "InputManager.h"
#include "Graphics.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "timerclass.h"
#include "Playerbot.h"

#define MAX_LOADSTRING 100

class GameEngine
{
public:
	GameEngine();
	~GameEngine();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	bool HandleInput(float dtime);
	void InitInstance(int&, int&);
	void ShutdownInstance();

	bool InitEntities();	// EDIT - new method to create startup entities

private:

	LPCWSTR m_applicationName;
	HINSTANCE m_hinst;								// current instance
	HWND m_hwnd;									// window handle
	InputManager* m_input;							// Gets keyboard and mouse input
	Graphics* m_graphics;							// Renders objects

	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	TimerClass* m_Timer;

	EntityManager* m_EntityManager;					// EDIT - Lists of gameworld objects
};

/////////////
// GLOBALS //
/////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static GameEngine* g_engine;
static const float PLAYER_SPEED = 300.0f; // EDIT - Player speed global
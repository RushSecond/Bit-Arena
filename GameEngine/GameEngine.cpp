// GameEngine.cpp
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html
// Edited by Blake Zoeckler

#include "stdafx.h"
#define _USE_MATH_DEFINES

#include "GameEngine.h"
#include "D2D1.h"
#include "DWrite.h"
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

GameEngine::GameEngine()
{
	m_input = 0;
	m_graphics = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_Timer = 0;

	// EDIT: intialize entity manager and player pointers
	m_EntityManager = 0;
}

GameEngine::~GameEngine()
{}

bool GameEngine::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitInstance(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard and mouse input from the user.
	m_input = new InputManager;
	if(!m_input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_input->Initialize(m_hinst, m_hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the graphics object.  This object will be used to render output to the screen.
	m_graphics = new Graphics;
	if(!m_graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	// ------ EDITS by Blake Zoeckler ----------
	// Get the entity manager from graphics
	m_EntityManager = m_graphics->GetEntityManager();

	// Add intial entities
	result = InitEntities();
	if (!result) return false;
	// -----------------------------------------

	// Create the fps object.
	m_Fps = new FpsClass;
	if(!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuClass;
	if(!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the timer object.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}
	
	return true;
}

// EDIT - new method that initializes entities for gameplay
bool GameEngine::InitEntities()
{
	bool result = true;
	Playerbot* player = 0;

	// Add entities. 0,0 is the center of the screen
	result &= m_EntityManager->AddEntity("default", ENTITYTYPE_PLAYER, 0, 0, 1.0f, ZeroVector, 0, true, (Entity**)&player);
	result &= m_EntityManager->AddEntity("arena", ENTITYTYPE_EVIRONMENT, 0, 0, 2.0f, ZeroVector, 0, true, NULL);
	result &= m_EntityManager->AddEntity("background", ENTITYTYPE_EVIRONMENT, 0, 0, 4.0f, ZeroVector, 0, true, NULL);

	if (!result) return false;

	// Add weapons to player
	result &= m_EntityManager->AddWeapon("playerBulletWeapon", "bullet", ENTITYTYPE_PLAYERBULLET, 1.0f, WEAPONTYPE_BULLET, 0.2f, 800.0f, 5.0f, 1, 0.0f);
	result &= m_EntityManager->AddWeapon("playerBeamWeapon", "flashy_beam", ENTITYTYPE_PLAYERBEAM, 2.0f, WEAPONTYPE_BEAM, 1.0f, 0.0f, 0.2f, 3, M_PI_4/4.0f);
	result &= m_EntityManager->AddWeapon("enemyBulletWeapon", "bullet", ENTITYTYPE_ENEMYBULLET, 1.0f, WEAPONTYPE_BULLET, 1.0f, 600.0f, 10.0f, 1, 0.0f);

	result &= m_EntityManager->AddWeaponToPlayer("playerBulletWeapon");

	result &= m_EntityManager->AddWeaponToBot(player, "playerBulletWeapon");
	result &= m_EntityManager->AddWeaponToBot(player, "playerBeamWeapon");

	return result;
}

void GameEngine::Shutdown()
{
	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the cpu object.
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the graphics object.
	if(m_graphics)
	{
		m_graphics->Shutdown();
		delete m_graphics;
		m_graphics = 0;
	}

	// Release the input object.
	if(m_input)
	{
		m_input->Shutdown();
		delete m_input;
		m_input = 0;
	}

	// Shutdown the window.
	ShutdownInstance();
	
	return;
}

void GameEngine::Run()
{
	MSG msg;
	bool done, result;
	
	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}

// Game Logic goes in this method
bool GameEngine::Frame()
{
	bool result;

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Calculate frame time
	float frameTime = m_Timer->GetTime() / 1000.0f;

	// Do input handling
	result = HandleInput(frameTime);
	if(!result) return false;

	// Do the frame processing for the entities.
	result = m_EntityManager->Frame(frameTime);
	if(!result)
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = m_graphics->Frame(m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), frameTime);
	if(!result)
	{
		return false;
	}

	return true;
}

bool GameEngine::HandleInput(float dtime)
{
	bool result;
	int mouseX, mouseY;
	Playerbot* player = m_EntityManager->GetPlayer();

	// Do the input frame processing.
	result = m_input->Frame();
	if(!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if(m_input->IsKeyDown(DIK_ESCAPE))
	{
		return false;
	}

	// Get the location of the mouse from the input object,
	m_input->GetMouseLocation(mouseX, mouseY);

	// Change location of cursor
	m_graphics->SetCursorPosition((float)mouseX, (float)mouseY);
	m_graphics->SetCameraShift((float)mouseX/3.0f, (float)mouseY/3.0f);

	float speed = PLAYER_SPEED;
	float xForce = 0.0f;
	float yForce = 0.0f;
	
	// Check if the user pressed arrow keys to move camera and player
	if(m_input->IsKeyDown(DIK_A))
	{
		xForce -= speed;
	}

	if(m_input->IsKeyDown(DIK_D))
	{
		xForce += speed;
	}

	if(m_input->IsKeyDown(DIK_W))
	{
		yForce += speed;
	}

	if(m_input->IsKeyDown(DIK_S))
	{
		yForce -= speed;
	}

	player->SetForce(xForce, yForce);

	// Camera follows the player
	float playerX, playerY;
	player->GetPosition(playerX, playerY);
	m_graphics->SetCameraPosition(playerX, playerY);

	// If left mouse is pressed, fire player's weapon
	if(m_input->MouseLeftPressed())
	{
		WeaponFireInfo info;
		
		if (player->CanFire())
		{
			// Determine bullet direction
			float cursorX, cursorY;
			m_graphics->GetCursorPosition(cursorX, cursorY);
			D3DXVECTOR2 direction = D3DXVECTOR2(cursorX - playerX, cursorY - playerY);

			// Fire to get bullets
			info = player->FireWeapon(direction);

			// Add the bullets to entity manager
			for (int i = 0; i < info.entitiesNum; i++)
				m_EntityManager->AddEntity((Entity*)*(info.entitiesOut + i));

			D3DXVECTOR2 playerPos(playerX, playerY);

			// Check collision of the rays
			for (int i = 0; i < info.raysNum; i++)
			{
				m_EntityManager->RayDamage(playerPos, *(info.raysOut + i), ENTITYTYPE_ENEMY, 3);
			}
		}
		
	}

	// Right click swaps player weapons
	if(m_input->MouseRightClick())
	{
		player->ChangeWeapon();
	}

	// Pressing left shift spawns an enemy
	if(m_input->IsKeyJustPressed(DIK_LSHIFT))
	{
		float cursorX, cursorY;
		Cyberbot* newEnemy;
		m_graphics->GetCursorPosition(cursorX, cursorY);

		m_EntityManager->AddEntity("enemy", ENTITYTYPE_ENEMY, cursorX, cursorY, 1.0f, ZeroVector, 0, true, (Entity**)&newEnemy);
		m_EntityManager->AddWeaponToBot(newEnemy, "enemyBulletWeapon");
	}

	return true;
}

LRESULT CALLBACK GameEngine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
void GameEngine::InitInstance(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.
	g_engine = this;

	// Get the instance of this application.
	m_hinst = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hinst;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_applicationName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowExW(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
				posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinst, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void GameEngine::ShutdownInstance()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinst);
	m_hinst = NULL;

	// Release the pointer to this class.
	g_engine = NULL;

	return;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wParam, LPARAM lParam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return g_engine->MessageHandler(hwnd, umessage, wParam, lParam);
		}
	}
}
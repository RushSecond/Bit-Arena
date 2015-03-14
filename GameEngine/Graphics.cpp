// Graphics.cpp
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html
// Edited by Blake Zoeckler

#include "stdafx.h"
#include "Graphics.h"

Graphics::Graphics(void)
{
	m_D3D = 0;
	m_Camera = 0;
	m_GUICamera = 0;
	m_Text = 0;
	m_TextureShader = 0;
	m_EntityManager = 0;
	m_ParticleShader = 0;
}


Graphics::~Graphics(void)
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
		
	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// Create the GUI camera object.
	m_GUICamera = new CameraClass;
	if(!m_GUICamera)
	{
		return false;
	}

	// Set the position of the GUI camera and render it
	m_GUICamera->SetPosition(0.0f, 0.0f, -10.0f);
	m_GUICamera->Render();

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the text object.
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Create the particle shader object.
	m_ParticleShader = new ParticleShaderClass;
	if(!m_ParticleShader)
	{
		return false;
	}

	// Initialize the particle shader object.
	result = m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the entity manager object.
	m_EntityManager = new EntityManager;
	if(!m_EntityManager) return false;

	// Initialize the entity manager object.
	result = m_EntityManager->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the entity manager object.", L"Error", MB_OK);
		return false;
	}

	InitializeEntities();

	return true;
}

// Initializes the entity manager
void Graphics::InitializeEntities()
{
	// Add images to the entity manager
	m_EntityManager->AddBitmap(L"../Textures/default.png", "default");
	m_EntityManager->AddBitmap(L"../Textures/cursor.png", "cursor");
	m_EntityManager->AddBitmap(L"../Textures/enemy.png", "enemy");
	m_EntityManager->AddBitmap(L"../Textures/bullet.png", "bullet");
	m_EntityManager->AddBitmap(L"../Textures/beam.png", "beam");
	m_EntityManager->AddBitmapAnimated(L"../Textures/flashybeam.png", 15, 1, "flashy_beam");
	m_EntityManager->AddBitmap(L"../Textures/arena.bmp", "arena");
	m_EntityManager->AddBitmap(L"../Textures/background.bmp", "background");

	// Add a cursor entity and get the handle to it
	m_EntityManager->AddEntity("cursor", ENTITYTYPE_GUI, 0, 0, 1.0f, D3DXVECTOR2(0,0), 0, true, &m_Cursor);
}

void Graphics::Shutdown()
{
	// Release the entity manager object
	if(m_EntityManager)
	{
		m_EntityManager->Shutdown();
		delete m_EntityManager;
		m_EntityManager = 0;
	}

	// Release the particle shader object.
	if(m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = 0;
	}

	// Release the text object.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the camera objects.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_GUICamera)
	{
		delete m_GUICamera;
		m_GUICamera = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool Graphics::Frame(int fps, int cpu, float frameTime)
{
	bool result;

	// Set the frames per second.
	result = m_Text->SetFps(fps, m_D3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// Set the cpu usage.
	result = m_Text->SetCpu(cpu, m_D3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}


bool Graphics::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, GUIMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_GUICamera->GetViewMatrix(GUIMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin 2D rendering
	m_D3D->TurnZBufferOff();
	m_D3D->TurnAlphaBlendingOn();

	// Entity manager renders all gameworld objects
	result = m_EntityManager->RenderEntities(m_D3D->GetDeviceContext(), m_TextureShader, worldMatrix, viewMatrix, orthoMatrix);
	if(!result)	return false;

	// Entity manager renders all GUI objects
	result = m_EntityManager->RenderGUI(m_D3D->GetDeviceContext(), m_TextureShader, worldMatrix, GUIMatrix, orthoMatrix);
	if(!result)	return false;

	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, GUIMatrix, orthoMatrix);
	if(!result)	return false;

	// Turn on particle blending
	m_D3D->TurnParticleBlendingOn();

	result = m_EntityManager->RenderParticles(m_D3D->GetDeviceContext(), m_ParticleShader, worldMatrix, viewMatrix, orthoMatrix);

	if(!result)
	{
		return false;
	}

	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

D3DXVECTOR3 Graphics::GetCameraPosition()
{
	return m_Camera->GetPosition();
}

void Graphics::SetCameraPosition(float x, float y)
{
	m_Camera->SetPosition(x, y);
}

void Graphics::TranslateCamera(float x, float y)
{
	m_Camera->Translate(x, y);
}

void Graphics::SetCameraShift(float x, float y)
{
	m_Camera->SetShift(x, y);
}

EntityManager* Graphics::GetEntityManager()
{
	return m_EntityManager;
}

void Graphics::SetCursorPosition(float x, float y)
{
	m_Cursor->SetPosition(D3DXVECTOR2(x,y));
}

void Graphics::GetCursorPosition(float& x, float& y)
{
	m_Cursor->GetPosition(x, y);
	D3DXVECTOR3 pos = m_Camera->GetPosition();

	x += pos.x;
	y += pos.y;
}
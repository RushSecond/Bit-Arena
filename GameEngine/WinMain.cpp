#include "stdafx.h"
#include "GameEngine.h"
#include <winerror.h>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	GameEngine* engine = new GameEngine();
	bool result;
	
	// Create the system object.
	engine = new GameEngine;
	if(!engine)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = engine->Initialize();
	if(result)
	{
		engine->Run();
	}

	// Shutdown and release the system object.
	engine->Shutdown();
	delete engine;
	engine = 0;

	return 0;
}
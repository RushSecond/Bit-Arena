#pragma once
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "dinput.h"

class InputManager
{
public:
	InputManager(void);
	~InputManager(void);
	bool Initialize(HINSTANCE, HWND, int, int);

	void Shutdown();
	bool Frame();

	bool IsKeyDown(unsigned int);
	bool IsKeyJustPressed(unsigned int);
	
	void GetMouseLocation(int&, int&);

	bool MouseLeftPressed();
	bool MouseLeftClick();

	bool MouseRightPressed();
	bool MouseRightClick();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	unsigned char m_keyboardPreviousState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
	int m_mouseDeltaX, m_mouseDeltaY;

	byte m_mouseLeft;
	bool m_mouseLeftClick;

	byte m_mouseRight;
	bool m_mouseRightClick;
};


#include "stdafx.h"
#include "InputManager.h"

InputManager::InputManager(void)
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputManager::~InputManager(void)
{
}

bool InputManager::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = 0; m_mouseY = 0;

	// Initialize mouse clicks to false
	m_mouseLeftClick = false; m_mouseLeft = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void InputManager::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputManager::Frame()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

bool InputManager::ReadKeyboard()
{
	HRESULT result;

	// Save previous keyboard state
	for (int i = 0; i < 256; i++)
		m_keyboardPreviousState[i] = m_keyboardState[i];

	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
		
	return true;
}

bool InputManager::ReadMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputManager::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY -= m_mouseState.lY;

	m_mouseDeltaX = m_mouseState.lX;
	m_mouseDeltaY = m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(m_mouseX < -m_screenWidth/2)  { m_mouseX = -m_screenWidth/2; }
	if(m_mouseY < -m_screenHeight/2)  { m_mouseY = -m_screenHeight/2; }
	
	if(m_mouseX > m_screenWidth/2)  { m_mouseX = m_screenWidth/2; }
	if(m_mouseY > m_screenHeight/2) { m_mouseY = m_screenHeight/2; }

	// Process left mouse click
	m_mouseLeftClick = (m_mouseState.rgbButtons[0] == 0x80 && m_mouseLeft != 0x80);
	m_mouseLeft = m_mouseState.rgbButtons[0];

	// Process right mouse click
	m_mouseRightClick = (m_mouseState.rgbButtons[1] == 0x80 && m_mouseRight != 0x80);
	m_mouseRight = m_mouseState.rgbButtons[1];
	
	return;
}

bool InputManager::IsKeyDown(unsigned int key)
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	return (m_keyboardState[key] == 0x80);
}

bool InputManager::IsKeyJustPressed(unsigned int key)
{
	// Do a bitwise and on the keyboard state and previous state
	// to check if the key is being pressed now and wasn't pressed last frame
	return (m_keyboardState[key] == 0x80 && m_keyboardPreviousState[key] != 0x80);
}

void InputManager::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

bool InputManager::MouseLeftPressed()
{
	return (m_mouseLeft == 0x80);
}

bool InputManager::MouseLeftClick()
{
	return m_mouseLeftClick;
}

bool InputManager::MouseRightPressed()
{
	return (m_mouseRight == 0x80);
}

bool InputManager::MouseRightClick()
{
	return m_mouseRightClick;
}


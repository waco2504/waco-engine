#pragma once

#ifndef _INC_INPUTSYSTEMDX_
#define _INC_INPUTSYSTEMDX_

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


/// DO POPRAWY KLASA!
class InputSystemDX {
	IDirectInput8* pdiInterface;
	LPDIRECTINPUTDEVICE8 pkeyboardDev;
	char keybuf[256];

	int convertToDIK(int key);
public:
	InputSystemDX(HWND hWnd);
	~InputSystemDX();

	void refresh();
	bool isPressed(int key);
	bool isPressedSpecial(int keyStroke);
	void AcquireKeyboard();
	void UnacquireKeyboard();
};

#endif
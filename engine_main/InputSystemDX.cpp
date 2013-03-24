#include "InputSystemDX.hpp"


int InputSystemDX::convertToDIK(int key) {

	return 0;
}

InputSystemDX::InputSystemDX(HWND hWnd) {
	if(FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**)&pdiInterface, NULL))) {
		MessageBox(NULL, "DirectInput8Create failed!", "Error!", MB_OK);
		exit(0);
	}

	if(FAILED(pdiInterface->CreateDevice(GUID_SysKeyboard, &pkeyboardDev, NULL))) {
		MessageBox(NULL, "pdiInterface->CreateDevice failed!", "Error!", MB_OK);
		exit(0);
	}

	pkeyboardDev->SetDataFormat(&c_dfDIKeyboard);
	
	pkeyboardDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND|DISCL_EXCLUSIVE);
}

InputSystemDX::~InputSystemDX() {
	pdiInterface->Release();
}

void InputSystemDX::refresh() {
	ZeroMemory(keybuf, sizeof(keybuf));
	pkeyboardDev->GetDeviceState(sizeof(keybuf), (LPVOID)&keybuf);
}

bool InputSystemDX::isPressed(int _k) {
	if(keybuf[_k] & 0x80) return true;
	return false;
}

bool InputSystemDX::isPressedSpecial(int _k) {
	return false;
}

void InputSystemDX::AcquireKeyboard() {
	pkeyboardDev->Acquire();
}

void InputSystemDX::UnacquireKeyboard() {
	pkeyboardDev->Unacquire();
}
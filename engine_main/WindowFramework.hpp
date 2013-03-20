#pragma once

#ifndef _INC_WINDOWFRAMEWORK_
#define _INC_WINDOWFRAMEWORK_

#include <Windows.h>
#include <string>

#include "Singleton.hpp"

// okno
class Window {
public: 
	struct WindowDescreption {
		unsigned int client_width;
		unsigned int client_height;

		HWND hWnd;
	};
private:
	Window(const Window&);
	Window& operator=(const Window&);
protected:
	WindowDescreption winDesc;
public:
	Window();

	void update();
	void showWindow(bool);
	
	bool isVisiable();
	const WindowDescreption getWindowInfo();

	static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

	friend class WindowFactory;
};

// Buduje okna
class WindowFactory : public Singleton<WindowFactory> {
public:
	Window* createWindow(std::string windowName, unsigned int windowWidth, unsigned int windowHeight);
	void deleteWindow(Window* window);
};

#endif
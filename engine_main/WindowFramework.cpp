#include "WindowFramework.hpp"


// Window
Window::Window(const Window&) {

}

Window& Window::operator=(const Window&) {
	return *this;
}

Window::Window() {

}

void Window::update() {
	MSG _msg;
	if(PeekMessage(&_msg, winDesc.hWnd, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&_msg);
		DispatchMessage(&_msg);
	}
}

void Window::showWindow(bool _show) {
	if(_show) ShowWindow(winDesc.hWnd, SW_SHOW);
	else ShowWindow(winDesc.hWnd, SW_HIDE);
}

bool Window::isVisiable() {
	return (bool)IsWindowVisible(winDesc.hWnd);
}

const Window::WindowDescreption Window::getWindowInfo() {
	return winDesc;
}

LRESULT Window::WndProc(HWND _hWnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	switch(_msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
	};
	return DefWindowProc(_hWnd, _msg, _wparam, _lparam);
}

// ---------------

// Window Factory
Window* WindowFactory::createWindow(std::string _name, unsigned int _width, unsigned int _height) {
	Window* newWindow = new Window();
	HINSTANCE _hInstance = GetModuleHandle(NULL);
	RECT _rect;

	newWindow->winDesc.client_height = _height;
	newWindow->winDesc.client_width = _width;


	_rect.left = 0;
	_rect.top = 0;
	_rect.bottom = _height;
	_rect.right = _width;

	AdjustWindowRect(&_rect, CS_HREDRAW | CS_VREDRAW, false);

	WNDCLASSEX windowClass;
	windowClass.cbSize			= sizeof(WNDCLASSEX);
	windowClass.style			= CS_HREDRAW | CS_VREDRAW; 
	windowClass.lpfnWndProc		= Window::WndProc;
	windowClass.cbWndExtra		= 0;
	windowClass.cbClsExtra		= 0;
	windowClass.hInstance		= _hInstance;
	windowClass.hIcon			= 0;
	windowClass.hIconSm			= 0;		  
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);		  
	windowClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1); 
	windowClass.lpszMenuName	= NULL;     
	windowClass.lpszClassName	= "engine_waco";

	RegisterClassEx(&windowClass);

	newWindow->winDesc.hWnd = 
		//CreateWindow("engine_waco", _name.c_str(), WS_OVERLAPPED|WS_SYSMENU,
		CreateWindow("engine_waco", _name.c_str(), WS_EX_TOPMOST|WS_POPUP|WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, _width, _height, 0, 0, _hInstance, 0);
			//CW_USEDEFAULT, CW_USEDEFAULT, _rect.right, _rect.bottom, 0, 0, _hInstance, 0);

	ShowWindow(newWindow->winDesc.hWnd,SW_SHOW);
	UpdateWindow(newWindow->winDesc.hWnd);


	return newWindow;
}

void WindowFactory::deleteWindow(Window* _win) {
	DestroyWindow(_win->winDesc.hWnd);
	UnregisterClass("engine_waco", GetModuleHandle(NULL));
}

// ---------------
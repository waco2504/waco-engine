#pragma once

#ifndef _DXERRORASSERT_HPP_
#define _DXERRORASSERT_HPP_

#include <Windows.h>

#ifdef _DEBUG
#define DXASSERT(x) do { if((x)) { std::string __str; __str += "File: "; __str += __FILE__; \
	__str += "\nLine: "; char __a[128] = {0}; _itoa_s(__LINE__,__a,128,10); __str += __a; __str += "\n"; __str += #x; \
	MessageBox(NULL, __str.c_str(), "Error", MB_OK); __debugbreak(); } } while(false);
#else 
#define DXASSERT(x) (x); //do{(void)sizeof((x));}while(false);
#endif

#endif
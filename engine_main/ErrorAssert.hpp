#pragma once

#ifndef _DXERRORASSERT_HPP_
#define _DXERRORASSERT_HPP_

#ifdef _DEBUG
#define DXASSERT(x) do { if((x)) { std::string str; str += "File: "; str += __FILE__; \
	str += "\nLine: "; char a[128] = {0}; _itoa_s(__LINE__,a,128,10); str += a; str += "\n"; str += #x; \
	MessageBox(NULL, str.c_str(), "Error", MB_OK); __debugbreak(); } } while(0);
#else 
#define DXASSERT(x) x;
#endif

#endif
#pragma once

#ifndef _DXERRORASSERT_HPP_
#define _DXERRORASSERT_HPP_

#include <D3D10.h>


#define DXASSERT(x) if(x!=0) { std::string str; str += "DX Error"; str += "\nFile: "; str += __FILE__; \
	str += "\nLine: "; char a[128] = {0}; _itoa_s(__LINE__,a,128,10); str += a; \
	MessageBox(NULL, str.c_str(), "", MB_OK); exit(1); }

#endif
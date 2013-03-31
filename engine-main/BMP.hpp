#ifndef _INC_BMP_
#define _INC_BMP_

#pragma once

#include <Windows.h>

struct BMPFILE {
	UINT width;
	UINT height;
	LONG size;
	BYTE* data;
};

long BMPLoadBmpFromFile(BMPFILE* out, const char* filePath);


#endif
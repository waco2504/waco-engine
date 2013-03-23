#include "BMP.hpp"


long BMPLoadBmpFromFile(BMPFILE* out, const char* filePath) {
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	DWORD bytesread;

	HANDLE file = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ,
		 NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	
	if(NULL == file) return 1; 
	
	if(ReadFile(file, &bmpheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false) {
		CloseHandle(file);
		return 1; 
	}

	if(ReadFile(file, &bmpinfo, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false) {
		CloseHandle(file);
		return 1;
	}
	
	if(bmpheader.bfType != 'MB') {
		CloseHandle(file);
		return 1;
	}

	out->width = bmpinfo.biWidth;
	out->height = abs(bmpinfo.biHeight);

	if(bmpinfo.biCompression != BI_RGB) {
		CloseHandle(file);
		return 1;
	}

	if(bmpinfo.biBitCount != 32) {
		CloseHandle(file);
		return 1;
	}
	
	out->size = bmpheader.bfSize - bmpheader.bfOffBits;

	out->data = new BYTE[out->size];

	SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);

	if (ReadFile(file, out->data, out->size, &bytesread, NULL) == false) {
		delete [] out->data;
		CloseHandle(file);
		return 1;
	}

	// BGRA -> RGBA i odwrocenie
	BYTE temp;

	for(UINT h = 0; h < out->height; ++h) {
		for(UINT w = 0; h < out->height/2 && w < out->width*4; ++w) {
			temp = out->data[h*out->width*4 + w];
			out->data[h*out->width*4 + w] = out->data[(out->height-h-1)*out->width*4 + w];
			out->data[(out->height-h-1)*out->width*4 + w] = temp;
		}
		for(UINT w = 0; w < out->width*4; w+=4) {
			temp = out->data[h*out->width*4 + w];
			out->data[h*out->width*4 + w] = out->data[h*out->width*4 + w + 2];
			out->data[h*out->width*4 + w + 2] = temp;
		}
	}


	CloseHandle(file);
	return 0;
}

#include "BMP.hpp"


long BMPLoadBmpFromFile(BMPFILE* out, const char* filePath) {
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	ZeroMemory(&bmpheader, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&bmpinfo, sizeof(BITMAPINFOHEADER));
	DWORD bytesread = 0;
	BYTE* rawData = NULL;
	DWORD rawDataSize = 0;

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

	if(bmpinfo.biBitCount != 32 && bmpinfo.biBitCount != 24) {
		CloseHandle(file);
		return 1;
	}
	
	rawDataSize = bmpheader.bfSize - bmpheader.bfOffBits;
	rawData = new BYTE[rawDataSize];

	SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);

	if (ReadFile(file, rawData, rawDataSize, &bytesread, NULL) == false) {
		delete [] rawData;
		CloseHandle(file);
		return 1;
	}

	if(bmpinfo.biBitCount == 24) {
		out->size = rawDataSize + (out->width * out->height);
		out->data = new BYTE[out->size];

		for(UINT i = 0, j = 0; i < (UINT)out->size; ++i) {
			if((i+1)%4 == 0) {
				out->data[i] = (BYTE)255;
			}
			else {
				out->data[i] = rawData[j];
				++j;
			}
		}

		delete [] rawData;
	}
	else {
		out->size = rawDataSize;
		out->data = rawData;
	}


	// BGRA -> RGBA i odwrocenie
	BYTE temp = 0;

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

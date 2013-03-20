#pragma once

#ifndef _INC_DXCLASS_
#define _INC_DXCLASS_

#include <D3D10.h>

class DXClass {
protected:
	ID3D10Device* pd3dDevice;
public:
	DXClass() : pd3dDevice(NULL) {}
	~DXClass() {}

	inline void setDxDevice(ID3D10Device* device) {
		pd3dDevice = device;
	}
	
	inline ID3D10Device* getDxDevice() {
		return pd3dDevice;
	}
};

#endif
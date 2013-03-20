#ifndef _INC_RESOURCEMENAGERDX10_
#define _INC_RESOURCEMENAGERDX10_

#include <string>
#include <map>

#include "DXClass.hpp"


class ResourceMenagerDX10 : public DXClass {
public:
	struct RESOURCE {
		union {
			ID3D10Texture2D* tex2d;
			ID3D10Buffer* buf;
		};

		ID3D10ShaderResourceView* srv;
		ID3D10RenderTargetView* rtv;
		ID3D10DepthStencilView* dsv;

		// pojedyncze face'y dla cubemap
		ID3D10ShaderResourceView* srvArray[6];
		ID3D10RenderTargetView* rtvArray[6];
		ID3D10DepthStencilView* dsvArray[6];
	};
private:
	std::map<std::string, RESOURCE> data;
public:
	// sprawdzac bo moze oplik zostal juz zaladowany jak tak to nie tworzyc nowego 
	// resourca tylko cos jak alias, inna nazwa ale wskazuje na ten sam resource
	void loadTexture2DFromFile(const std::string& name, const char* fpath);
	void createTexture2D(const std::string& name, UINT width, UINT height, 
		UINT genMips, DXGI_FORMAT format, int bindFlags);
	void createTextureCube(const std::string& name, UINT width, UINT height, 
		UINT genMips, DXGI_FORMAT format, int bindFlags);
	void createVertexBuffer(const std::string& name, UINT bytes, 
		const void* data);
	void createIndexBuffer(const std::string& name, UINT bytes, 
		const void* data);
	
	RESOURCE* getData(const std::string& name);
};

#endif
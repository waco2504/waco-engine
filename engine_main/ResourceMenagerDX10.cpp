#include "ResourceMenagerDX10.hpp"

#include "DXErrorAssert.hpp"

#include <d3dx10.h>

void ResourceMenagerDX10::loadTexture2DFromFile(const std::string& name,
	const char* fpath) {
		
	data[name];
	ZeroMemory(&data[name], sizeof(RESOURCE));
	HRESULT hr =  D3DX10CreateTextureFromFile(pd3dDevice, fpath, NULL, NULL,
 		(ID3D10Resource**)&data[name].tex2d, NULL);
	
	DXASSERT(hr);

	D3D10_TEXTURE2D_DESC ltd;
	data[name].tex2d->GetDesc(&ltd);

	D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = ltd.Format;
	srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = ltd.MipLevels;

	DXASSERT(pd3dDevice->CreateShaderResourceView
			(data[name].tex2d, &srvd, &data[name].srv));

}

void ResourceMenagerDX10::createTexture2D(const std::string& name, UINT w, 
	UINT h, UINT gm, DXGI_FORMAT format, int bindFlags) {

	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = gm;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = bindFlags;

	data[name];
	ZeroMemory(&data[name], sizeof(RESOURCE));

	DXASSERT(pd3dDevice->CreateTexture2D(&desc, NULL, &data[name].tex2d));


	if(bindFlags & D3D10_BIND_SHADER_RESOURCE) {
		D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC));
		srvd.Format = format;
		srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = gm;

		DXASSERT(pd3dDevice->CreateShaderResourceView
			(data[name].tex2d, &srvd, &data[name].srv));
	}
	if(bindFlags & D3D10_BIND_RENDER_TARGET) {
		D3D10_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(D3D10_RENDER_TARGET_VIEW_DESC));
		rtvd.Format = format;
		rtvd.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;

		DXASSERT(pd3dDevice->CreateRenderTargetView
			(data[name].tex2d, &rtvd, &data[name].rtv)); 
	}
	if(bindFlags & D3D10_BIND_DEPTH_STENCIL) {
		D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D10_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = format;
		dsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;

		DXASSERT(pd3dDevice->CreateDepthStencilView
			(data[name].tex2d, &dsvd, &data[name].dsv)); 
	}
}

void ResourceMenagerDX10::createTextureCube(const std::string& name, UINT w, 
	UINT h, UINT gm, DXGI_FORMAT format, int bindFlags) {

	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = gm;
	desc.ArraySize = 6;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = bindFlags;
	desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
	
	data[name];
	ZeroMemory(&data[name], sizeof(RESOURCE));

	DXASSERT(pd3dDevice->CreateTexture2D(&desc, NULL, &data[name].tex2d));


	if(bindFlags & D3D10_BIND_SHADER_RESOURCE) {
		D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC));
		srvd.Format = format;
		srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
		srvd.TextureCube.MipLevels = gm;

		DXASSERT(pd3dDevice->CreateShaderResourceView
			(data[name].tex2d, &srvd, &data[name].srv));
	}
	if(bindFlags & D3D10_BIND_RENDER_TARGET) {
		D3D10_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.Format = format;
		rtvd.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvd.Texture2DArray.FirstArraySlice = 0;
		rtvd.Texture2DArray.ArraySize = 6;
		rtvd.Texture2DArray.MipSlice = 0;
		pd3dDevice->CreateRenderTargetView(data[name].tex2d, &rtvd, &data[name].rtv);

		rtvd.Texture2DArray.ArraySize = 1;
		for(unsigned int i = 0; i < 6; ++i) {
			rtvd.Texture2DArray.FirstArraySlice = i;
			DXASSERT(pd3dDevice->CreateRenderTargetView
				(data[name].tex2d, &rtvd, &data[name].rtvArray[i]));
		}
	}
	if(bindFlags & D3D10_BIND_DEPTH_STENCIL) {
		D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
		dsvd.Format = format;
		dsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvd.Texture2DArray.FirstArraySlice = 0;
		dsvd.Texture2DArray.ArraySize = 6;
		dsvd.Texture2DArray.MipSlice = 0;
		pd3dDevice->CreateDepthStencilView(data[name].tex2d, &dsvd, &data[name].dsv);

		dsvd.Texture2DArray.ArraySize = 1;
		for(unsigned int i = 0; i < 6; ++i) {
			dsvd.Texture2DArray.FirstArraySlice = i;
			DXASSERT(pd3dDevice->CreateDepthStencilView
				(data[name].tex2d, &dsvd, &data[name].dsvArray[i]));
		}
	}
}

void ResourceMenagerDX10::createVertexBuffer(const std::string& name, 
	UINT bytes, const void* inidata) {

	D3D10_BUFFER_DESC desc;
	desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	desc.ByteWidth = bytes;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = NULL;
	desc.Usage = D3D10_USAGE_DEFAULT;

	D3D10_SUBRESOURCE_DATA memdata;
	memdata.pSysMem = inidata;
	memdata.SysMemPitch = 0;
	memdata.SysMemSlicePitch = 0;

	ZeroMemory(&data[name], sizeof(RESOURCE));

	DXASSERT(pd3dDevice->CreateBuffer(&desc, &memdata, &data[name].buf));
}

void ResourceMenagerDX10::createIndexBuffer(const std::string& name, UINT bytes, 
	const void* inidata) {

	D3D10_BUFFER_DESC desc;
	desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	desc.ByteWidth = bytes;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = NULL;
	desc.Usage = D3D10_USAGE_DEFAULT;

	D3D10_SUBRESOURCE_DATA memdata;
	memdata.pSysMem = inidata;
	memdata.SysMemPitch = 0;
	memdata.SysMemSlicePitch = 0;

	ZeroMemory(&data[name], sizeof(RESOURCE));

	DXASSERT(pd3dDevice->CreateBuffer(&desc, &memdata, &data[name].buf));
}

ResourceMenagerDX10::RESOURCE* ResourceMenagerDX10::getData
	(const std::string& name) {
	return &data[name];
}
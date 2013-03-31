#include "ResourceMenagerDX10.hpp"

#include "BMP.hpp"
#include "ErrorAssert.hpp"

/* Funkcja ³aduje texture z pliku i inicjalizuje dx resource */
void ResourceMenagerDX10::loadTexture2DFromFile(const std::string name, const char* fpath) {
	HRESULT hr = S_OK;
	BMPFILE bmp;
	D3D10_SUBRESOURCE_DATA texdata;
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&texdata, sizeof(texdata));
	ZeroMemory(&bmp, sizeof(bmp));

	DXASSERT(data.find(name) != data.end());
	
	hr = BMPLoadBmpFromFile(&bmp, fpath);
	DXASSERT(hr != 0);

	desc.Width = bmp.width;
	desc.Height = bmp.height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	data[name];
	ZeroMemory(&data[name], sizeof(RESOURCE));

	texdata.pSysMem = bmp.data;
	texdata.SysMemPitch = bmp.width * 4;

	hr = pd3dDevice->CreateTexture2D(&desc, &texdata, &data[name].tex2d);
	DXASSERT(S_OK != hr);

	D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;

	hr = pd3dDevice->CreateShaderResourceView(data[name].tex2d, &srvd, &data[name].srv);
	DXASSERT(S_OK != hr);
	
	//delete [] bmp.data;
}

void ResourceMenagerDX10::createTexture2D(const std::string name, UINT w, 
	UINT h, UINT gm, DXGI_FORMAT format, int bindFlags) {
		HRESULT hr = S_OK;
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = gm;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.BindFlags = bindFlags;

		DXASSERT(data.find(name) != data.end());
		data[name];
		ZeroMemory(&data[name], sizeof(RESOURCE));

		hr = pd3dDevice->CreateTexture2D(&desc, NULL, &data[name].tex2d);
		DXASSERT(S_OK != hr);


		if(bindFlags & D3D10_BIND_SHADER_RESOURCE) {
			D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
			ZeroMemory(&srvd, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC));
			srvd.Format = format;
			srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MipLevels = gm;

			hr = pd3dDevice->CreateShaderResourceView(data[name].tex2d, &srvd, &data[name].srv);
			DXASSERT(S_OK != hr);
		}
		if(bindFlags & D3D10_BIND_RENDER_TARGET) {
			D3D10_RENDER_TARGET_VIEW_DESC rtvd;
			ZeroMemory(&rtvd, sizeof(D3D10_RENDER_TARGET_VIEW_DESC));
			rtvd.Format = format;
			rtvd.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;

			hr = pd3dDevice->CreateRenderTargetView(data[name].tex2d, &rtvd, &data[name].rtv);
			DXASSERT(S_OK != hr); 
		}
		if(bindFlags & D3D10_BIND_DEPTH_STENCIL) {
			D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
			ZeroMemory(&dsvd, sizeof(D3D10_DEPTH_STENCIL_VIEW_DESC));
			dsvd.Format = format;
			dsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;

			hr = pd3dDevice->CreateDepthStencilView(data[name].tex2d, &dsvd, &data[name].dsv);
			DXASSERT(S_OK != hr); 
		}
}

void ResourceMenagerDX10::createTextureCube(const std::string name, UINT w, 
	UINT h, UINT gm, DXGI_FORMAT format, int bindFlags) {
		HRESULT hr = S_OK;
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

		hr = pd3dDevice->CreateTexture2D(&desc, NULL, &data[name].tex2d);
		DXASSERT(S_OK != hr);


		if(bindFlags & D3D10_BIND_SHADER_RESOURCE) {
			D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
			ZeroMemory(&srvd, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC));
			srvd.Format = format;
			srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
			srvd.TextureCube.MipLevels = gm;

			hr = pd3dDevice->CreateShaderResourceView(data[name].tex2d, &srvd, &data[name].srv);
			DXASSERT(S_OK != hr);
		}
		if(bindFlags & D3D10_BIND_RENDER_TARGET) {
			D3D10_RENDER_TARGET_VIEW_DESC rtvd;
			ZeroMemory(&rtvd, sizeof(D3D10_RENDER_TARGET_VIEW_DESC));
			rtvd.Format = format;
			rtvd.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvd.Texture2DArray.FirstArraySlice = 0;
			rtvd.Texture2DArray.ArraySize = 6;
			rtvd.Texture2DArray.MipSlice = 0;
			
			hr = pd3dDevice->CreateRenderTargetView(data[name].tex2d, &rtvd, &data[name].rtv);
			DXASSERT(S_OK != hr);

			rtvd.Texture2DArray.ArraySize = 1;
			for(unsigned int i = 0; i < 6; ++i) {
				rtvd.Texture2DArray.FirstArraySlice = i;
				hr = pd3dDevice->CreateRenderTargetView(data[name].tex2d, &rtvd, &data[name].rtvArray[i]);
				DXASSERT(S_OK != hr);
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
				hr = pd3dDevice->CreateDepthStencilView(data[name].tex2d, &dsvd, &data[name].dsvArray[i]);
				DXASSERT(S_OK != hr);
			}
		}
}

void ResourceMenagerDX10::createVertexBuffer(const std::string name, 
	UINT bytes, const void* inidata) {
		HRESULT hr = S_OK;
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

		hr = pd3dDevice->CreateBuffer(&desc, &memdata, &data[name].buf);
		DXASSERT(S_OK != hr);
}

void ResourceMenagerDX10::createIndexBuffer(const std::string name, UINT bytes, 
	const void* inidata) {
		HRESULT hr = S_OK;
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

		hr = pd3dDevice->CreateBuffer(&desc, &memdata, &data[name].buf);
		DXASSERT(S_OK != hr);
}

ResourceMenagerDX10::RESOURCE* ResourceMenagerDX10::getData(const std::string name) {
	return &data[name];
}
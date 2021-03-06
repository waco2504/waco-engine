#include "RendererDX10.hpp"
#include "ErrorAssert.hpp"

typedef MESHDX10::EVERTEX EVERTEX;

bool operator==(const RENDERABLE& l, const RENDERABLE& r) {
	return l.World == r.World && l.Mesh == r.Mesh && l.Material == r.Material;
}

bool operator==(const LIGHTDX10& l, const LIGHTDX10& r) {
	return l.lightName == r.lightName;
}

bool operator==(const RENDERBATCH& l, const RENDERBATCH& r) {
	bool ret = l.shaderType == r.shaderType && l.world == r.world  
		&& l.mesh == r.mesh && l.light == r.light;
	
	if(ret) {
		for(unsigned int i = 0; i < SHADERMAXSRV; ++i) {
			if(l.srvs[i] != r.srvs[i]) return false;
		}
	}

	return ret;
}

struct CMPLIGHTTYPE {
	bool operator()(const LIGHTDX10* a, const LIGHTDX10* b) {
		return a->Type < b->Type;
	}
} cmpLightType;

struct CMPRENDERBATCH { // a < b
	bool operator()(const RENDERBATCH& a, const RENDERBATCH& b) {
		if(a.shaderType < b.shaderType) return true;
		else if(a.shaderType == b.shaderType) {
			EVECTOR4 cP = a.camPos;
			EVECTOR4 ca = a.mesh->BB[0] + a.mesh->BB[7];
			EVECTOR4 cb = b.mesh->BB[0] + b.mesh->BB[7];
			ca /= 2.0f;
			cb /= 2.0f;

			return (ca-cP).length() < (cb-cP).length();
		}
		else return false;
	}
} cmpRenderBatch;

void RendererDX10::cullBatches(const EFRUSTUM fr) {
	visibleScene.clear();
	EVECTOR3 vp, vn, minValues, maxValues, AABB[6];
	bool inside = true, nullfr = false;
	float distp = 0.0f, distn = 0.0f;


	for(unsigned int i = 0; i < fullScene.size(); ++i) { // dla ka�dego batcha
		inside = true;
		minValues = EVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
		maxValues = EVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		
		for(unsigned int p = 0; p < 6; ++p) {
			AABB[p] = fullScene[i]->AABB[p];
			for(unsigned int t = 0; t < 3; ++t) {
				if(minValues[t] > AABB[p][t]) minValues[t] = AABB[p][t];
				if(maxValues[t] < AABB[p][t]) maxValues[t] = AABB[p][t];
			}
		}

		for(unsigned int j = 0; j < 6; ++j) {
			vp = minValues;
			vn = maxValues;

			if(fr.plane[j].a > -0.0f) vp.x = maxValues.x;
			if(fr.plane[j].b > -0.0f) vp.y = maxValues.y;
			if(fr.plane[j].c > -0.0f) vp.z = maxValues.z;

			if(fr.plane[j].a > -0.0f) vn.x = minValues.x;
			if(fr.plane[j].b > -0.0f) vn.y = minValues.y;
			if(fr.plane[j].c > -0.0f) vn.z = minValues.z;

			distp = (vp.x * fr.plane[j].a) 
				 + (vp.y * fr.plane[j].b)
				 + (vp.z * fr.plane[j].c)
				 + fr.plane[j].d;

			distn = (vn.x * fr.plane[j].a) 
				 + (vn.y * fr.plane[j].b)
				 + (vn.z * fr.plane[j].c)
				 + fr.plane[j].d;

			if(distp < 0 && distn < 0) { // obapunkty poza
				inside = false;
				break;
			}
		}

		if(inside) {
			visibleScene.push_back(i);
		}
	}
}

void RendererDX10::initBackBufferRT() {
	HRESULT hr = S_OK;
	D3D10_TEXTURE2D_DESC ddesc;
	ID3D10Texture2D* prt = NULL;
	D3D10_DEPTH_STENCIL_VIEW_DESC dsdesc;
	ZeroMemory(&dsdesc, sizeof(dsdesc));

	D3D10_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	hr = pswapChain->GetBuffer(0, __uuidof(prt), reinterpret_cast<void**>(&prt));
	DXASSERT(hr != S_OK);

	hr = pd3dDevice->CreateRenderTargetView(prt, &desc, &pbbRT);
	DXASSERT(hr != S_OK);

	prt->GetDesc(&ddesc);

	prt->Release();

	ddesc.ArraySize = 1;
	ddesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	ddesc.CPUAccessFlags = 0;
	ddesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ddesc.MipLevels = 1;
	ddesc.MiscFlags = 0;
	ddesc.SampleDesc.Count = 1;
	ddesc.SampleDesc.Quality = 0;
	ddesc.Usage = D3D10_USAGE_DEFAULT;

	hr = pd3dDevice->CreateTexture2D(&ddesc, NULL, &pbbDRes);
	DXASSERT(hr != S_OK);

	dsdesc.Format = DXGI_FORMAT_UNKNOWN;
	dsdesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsdesc.Texture2D.MipSlice = 0;

	hr = pd3dDevice->CreateDepthStencilView(pbbDRes, &dsdesc, &pbbD);
	DXASSERT(hr != S_OK);
}

void RendererDX10::initShadowDepthBuffer() {
	for(int i = 64; i <= 1024; i*=2) {
		resMen->createTexture2D(getShadowMapName(false,i), i, i, 1, 
			DXGI_FORMAT_D24_UNORM_S8_UINT, D3D10_BIND_DEPTH_STENCIL);

		resMen->createTextureCube(getShadowMapName(true,i), i, i, 1, 
			DXGI_FORMAT_D24_UNORM_S8_UINT, D3D10_BIND_DEPTH_STENCIL);
	}
}

void RendererDX10::initDepthStencilState() {
	D3D10_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D10_COMPARISON_LESS;

	dsDesc.StencilEnable = false;

	pd3dDevice->CreateDepthStencilState(&dsDesc, &pdepthStencilStateDef);

	dsDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;

	pd3dDevice->CreateDepthStencilState(&dsDesc, &pdepthStencilStateLessEq);

	dsDesc.DepthFunc = D3D10_COMPARISON_EQUAL;

	pd3dDevice->CreateDepthStencilState(&dsDesc, &pdepthStencilStateEq);

	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
	
	pd3dDevice->CreateDepthStencilState(&dsDesc, &pdepthStencilStateEqOff);
	
	dsDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	pd3dDevice->CreateDepthStencilState(&dsDesc, &pdepthStencilStateLessEqOff);

	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D10_COMPARISON_ALWAYS;
	pd3dDevice->CreateDepthStencilState(&dsDesc, &pdepthStencilStateOffOff);
}

void RendererDX10::initBlendState() {
	D3D10_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D10_BLEND_DESC));
	BlendState.BlendEnable[0] = TRUE;
	BlendState.BlendOp = D3D10_BLEND_OP_ADD;
	BlendState.SrcBlend = D3D10_BLEND_ONE;
	BlendState.DestBlend = D3D10_BLEND_ONE;
	BlendState.SrcBlendAlpha = D3D10_BLEND_ONE;
	BlendState.DestBlendAlpha = D3D10_BLEND_ONE;
	BlendState.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	BlendState.AlphaToCoverageEnable = FALSE;
	BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	
	pd3dDevice->CreateBlendState(&BlendState, &pblendStateAdd);

	BlendState.BlendEnable[0] = FALSE;
	BlendState.RenderTargetWriteMask[0] = FALSE;
	pd3dDevice->CreateBlendState(&BlendState, &pblendStateColorOff);
}

void RendererDX10::initResterState() {
	D3D10_RASTERIZER_DESC rasterizerState;
    rasterizerState.FillMode = D3D10_FILL_SOLID;
    rasterizerState.CullMode = D3D10_CULL_BACK;
    rasterizerState.FrontCounterClockwise = false;
    rasterizerState.DepthBias = 0;
    rasterizerState.DepthBiasClamp = 0;
    rasterizerState.SlopeScaledDepthBias = 0;
    rasterizerState.DepthClipEnable = true;
    rasterizerState.ScissorEnable = false;
    rasterizerState.MultisampleEnable = false;
    rasterizerState.AntialiasedLineEnable = false;
    pd3dDevice->CreateRasterizerState(&rasterizerState, &presterStateSolid);

	rasterizerState.FillMode = D3D10_FILL_WIREFRAME;
    rasterizerState.CullMode = D3D10_CULL_NONE;
	pd3dDevice->CreateRasterizerState(&rasterizerState, &presterStateWireframe);

	rasterizerState.FillMode = D3D10_FILL_SOLID;
    rasterizerState.CullMode = D3D10_CULL_FRONT;
	pd3dDevice->CreateRasterizerState(&rasterizerState, &presterStateSolidCullFront);

    rasterizerState.CullMode = D3D10_CULL_NONE;
	pd3dDevice->CreateRasterizerState(&rasterizerState, &presterStateSolidCullOff);
}

void RendererDX10::initFullScreenQuad() {
	unsigned int ibData[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
	};

	EVERTEX vbData[] = {
		{ EVECTOR3(-1,1,0), EVECTOR3(0,0,0), EVECTOR4(0,0,0), EVECTOR3(0,0,0), },
		{ EVECTOR3(-1,-1,0), EVECTOR3(0,1,0), EVECTOR4(0,1,0), EVECTOR3(0,1,0), },
		{ EVECTOR3(1,1,0), EVECTOR3(1,0,0), EVECTOR4(1,0,0), EVECTOR3(1,0,0), },

		{ EVECTOR3(1,1,0), EVECTOR3(1,0,0), EVECTOR4(1,0,0), EVECTOR3(1,0,0), },
		{ EVECTOR3(-1,-1,0), EVECTOR3(0,1,0), EVECTOR4(0,1,0), EVECTOR3(0,1,0), },
		{ EVECTOR3(1,-1,0), EVECTOR3(1,1,0), EVECTOR4(1,1,0), EVECTOR3(1,1,0), },
	};

	resMen->createIndexBuffer(std::string("FullScreenQuadIB"),
		12*sizeof(unsigned int), ibData);

	resMen->createVertexBuffer(std::string("FullScreenQuadVB"), 
		6*sizeof(EVERTEX), vbData);

	fullScreenQuad.pdxindexBuffer = resMen->getData("FullScreenQuadIB")->buf;
	fullScreenQuad.pdxvertexBuffer = resMen->getData("FullScreenQuadVB")->buf;
	fullScreenQuad.indexCount = 12;
	fullScreenQuad.vertexStart = 0;
	fullScreenQuad.indexStart = 0;
}

void RendererDX10::initSkyBox() {
	EVERTEX sbVB[] = {
		{EVECTOR3(-1,1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,0,0),},
		{EVECTOR3(-1,-1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,1,0),},
		{EVECTOR3(1,-1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,1,0),},
		{EVECTOR3(1,1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,0,0),},

		{EVECTOR3(-1,1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,0,0),}, 
		{EVECTOR3(-1,-1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,1,0),},
		{EVECTOR3(1,-1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,1,0),},
		{EVECTOR3(1,1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,0,0),},


		{EVECTOR3(-1,1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,0,0),}, 
		{EVECTOR3(-1,-1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,1,0),},
		{EVECTOR3(1,-1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,1,0),},
		{EVECTOR3(1,1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,0,0),},

		{EVECTOR3(-1,1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,0,0),},
		{EVECTOR3(-1,-1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,1,0),},
		{EVECTOR3(1,-1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,1,0),},
		{EVECTOR3(1,1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,0,0),},


		{EVECTOR3(-1,1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,0,0),}, 
		{EVECTOR3(-1,-1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,1,0),},
		{EVECTOR3(1,-1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,1,0),},
		{EVECTOR3(1,1,1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,0,0),},

		{EVECTOR3(-1,1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,1,0),},
		{EVECTOR3(-1,-1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(1,0,0),},
		{EVECTOR3(1,-1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,0,0),},
		{EVECTOR3(1,1,-1), EVECTOR3(), EVECTOR4(), EVECTOR3(0,1,0),},
	};

	unsigned int sbIB[] = { 
		0, 1, 3, 1, 2, 3,		//+z
		4, 7, 5, 7, 6, 5,		//-z
		15, 11, 10, 10, 14, 15,	//+x
		8, 12, 13, 13, 9, 8,	//-x
		20, 16, 19, 19, 23, 20,	//+y
		17, 21, 22, 22, 18, 17,	//-y
	};

	resMen->createIndexBuffer("SkyBoxIB", 36*sizeof(unsigned int), sbIB);
	resMen->createVertexBuffer("SkyBoxVB", 24*sizeof(EVERTEX), sbVB);

	for(unsigned int i = 0; i < 6; ++i) {
		skyBoxFace[i].pdxindexBuffer = resMen->getData("SkyBoxIB")->buf;
		skyBoxFace[i].pdxvertexBuffer = resMen->getData("SkyBoxVB")->buf;
		skyBoxFace[i].indexCount = 6;
		skyBoxFace[i].vertexStart = 0;
		skyBoxFace[i].indexStart = i*6;
	}
}

void RendererDX10::init(HWND hWnd, UINT clientWidth, UINT clientHeight, bool isFullscreen) { 
	HRESULT hr = 0;
	DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Width = clientWidth;
	scd.BufferDesc.Height = clientHeight;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
	if(isFullscreen) scd.Windowed = false;
    else scd.Windowed = true;

#ifdef _DEBUG
	hr = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 
		D3D10_CREATE_DEVICE_SINGLETHREADED|D3D10_CREATE_DEVICE_DEBUG, 
		D3D10_SDK_VERSION, &scd, &pswapChain, &pd3dDevice);
#else
	hr = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 
		D3D10_CREATE_DEVICE_SINGLETHREADED, 
		D3D10_SDK_VERSION, &scd, &pswapChain, &pd3dDevice);
#endif

	DXASSERT(S_OK != hr);

	resMen = new ResourceMenagerDX10();
	resMen->setDxDevice(pd3dDevice);

	shaderMen = new ShaderMenagerDX10();
	shaderMen->setDxDevice(pd3dDevice);
	shaderMen->init();

	initBackBufferRT();
	initShadowDepthBuffer();
	initDepthStencilState();
	initBlendState();
	initResterState();
	initFullScreenQuad();
	initSkyBox();

	resMen->createTexture2D(std::string("ColorRenderTarget"), 
		clientWidth, clientHeight, 1, 
		DXGI_FORMAT_R8G8B8A8_UNORM, D3D10_BIND_RENDER_TARGET
		|D3D10_BIND_SHADER_RESOURCE);
	resMen->createTexture2D(std::string("LightRenderTarget"), 
		clientWidth, clientHeight, 1, 
		DXGI_FORMAT_R8G8B8A8_UNORM, D3D10_BIND_RENDER_TARGET
		|D3D10_BIND_SHADER_RESOURCE);
	resMen->createTexture2D(std::string("AORenderTarget"), 
		clientWidth, clientHeight, 1, 
		DXGI_FORMAT_R32_FLOAT, D3D10_BIND_RENDER_TARGET
		|D3D10_BIND_SHADER_RESOURCE);
	resMen->createTexture2D(std::string("AORenderTarget2"),
		clientWidth/2, clientHeight/2, 1, 
		DXGI_FORMAT_R32_FLOAT, D3D10_BIND_RENDER_TARGET
		|D3D10_BIND_SHADER_RESOURCE);
	resMen->createTexture2D(std::string("SSNormals"), 
		clientWidth, clientHeight,
		1, DXGI_FORMAT_R32G32B32A32_FLOAT, 
		D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE);
	resMen->loadTexture2DFromFile(std::string("SSRndNormals"), 
		"noise.bmp");

	setDefaultRenderDescreption();
}

void RendererDX10::setDefaultRenderDescreption() {
	renderDesc.renderstate = RENDERDESCREPTION::APPLYSSAO | RENDERDESCREPTION::APPLYLIGHT 
		| RENDERDESCREPTION::APPLYCOLOR | RENDERDESCREPTION::APPLYSHADOWS;
	renderDesc.shadowQuality = RENDERDESCREPTION::POINT;
}

void RendererDX10::setRenderDescreption(const RENDERDESCREPTION& desc) {
	renderDesc = desc;
}

void RendererDX10::setCamera(const CAMERADX10 pcamera) {
	curCam = pcamera;
}

void RendererDX10::prepareSMBatches() {
	RENDERBATCH batch;
	RENDERBATCHPACK pack;
	int lt = 0;
	bool bump = false;
	bool shadow = false;
	float col[] = {0.0f,0.0f,0.0f,0.0f};

	// shadow mapy
	if(renderDesc.renderstate & RENDERDESCREPTION::APPLYSHADOWS) {
		for(unsigned int li = 0; li < lights.size(); ++li) {
			pack.rtv = lights[li]->pShadowMapRTV;

			if(lights[li]->Type == LIGHTDX10::POINT) {
				pack.dsv = resMen->getData(getShadowMapName(true, 
					lights[li]->ShadowMapSize))->dsv;
			}
			else if(lights[li]->Type == LIGHTDX10::SPOT) {
				pack.dsv = resMen->getData(getShadowMapName(false, 
					lights[li]->ShadowMapSize))->dsv;
			}
			pd3dDevice->ClearRenderTargetView(pack.rtv, col);
			pd3dDevice->ClearDepthStencilView(pack.dsv, D3D10_CLEAR_DEPTH, 1.0f, 0);

			pack.viewPort = lights[li]->ViewPort;
			pack.blendState = NULL;
			pack.depthState = pdepthStencilStateDef;
			pack.resterState = presterStateSolidCullOff;
	
			if(lights[li]->Type == LIGHTDX10::SPOT) {
				cullBatches(lights[li]->Frustum[0]);
			} 
			else if(lights[li]->Type == LIGHTDX10::POINT) {
				visibleScene.clear();
				for(unsigned int i = 0; i < fullScene.size(); ++i) {
					visibleScene.push_back(i);
				}
			}

			pack.batches.clear();
			for(unsigned int oi = 0; oi < visibleScene.size(); ++oi) {
				batch.camPos = lights[li]->Position;
				batch.color = lights[li]->Color;
				batch.light.Color = lights[li]->Color;
				batch.light.Direction = lights[li]->Direction;
				batch.light.Parameters = lights[li]->Parameters;
				batch.light.Position = lights[li]->Position;
				batch.light.Proj = lights[li]->Proj;
				batch.light.View = lights[li]->View[0];
				batch.mesh = fullScene[visibleScene[oi]]->Mesh;
				batch.proj = batch.light.Proj;
			
				if(lights[li]->Type == LIGHTDX10::SPOT)
					batch.shaderType = SHADERSETDX10::SHADOWMAP2D;
				else if(lights[li]->Type == LIGHTDX10::POINT) 
					batch.shaderType = SHADERSETDX10::SHADOWMAPCUBE;
			
				for(unsigned int i = 0; i < SHADERMAXSRV; ++i) 
					batch.srvs[i] = NULL;
			
				for(unsigned int i = 0; i < 6; ++i)
					batch.view[i] = lights[li]->View[i];
			
				batch.world = fullScene[visibleScene[oi]]->World;
			
				pack.batches.push_back(batch);
			}
			std::sort(pack.batches.begin(), pack.batches.end(), cmpRenderBatch);
			batchPacks.push_back(pack);
		}
	}
}

void RendererDX10::prepareColBatches() {
	RENDERBATCH batch;
	RENDERBATCHPACK pack;
	int lt = 0;
	bool bump = false;
	bool shadow = false;
	float col[] = {0.0f,0.0f,0.0f,0.0f};
	
	pack.rtv = resMen->getData(std::string("ColorRenderTarget"))->rtv;
	pack.dsv = pbbD;
	pd3dDevice->ClearRenderTargetView(pack.rtv, col);
	pd3dDevice->ClearDepthStencilView(pack.dsv, D3D10_CLEAR_DEPTH, 1.0f, 0);
	pack.viewPort = curCam.ViewPort;
	
	if(renderDesc.renderstate & RENDERDESCREPTION::APPLYCOLOR) {
		pack.blendState = NULL;
	}
	else {
		pack.blendState = pblendStateColorOff;
	}

	if(renderDesc.renderstate & RENDERDESCREPTION::APPLYSKYBOX) {
		EMATRIX mat;
		mat.identity();
		mat.translate(curCam.Position.x, curCam.Position.y, curCam.Position.z);
		
		pack.depthState = pdepthStencilStateOffOff;

		if(renderDesc.renderstate & RENDERDESCREPTION::APPLYWIREFRAME) {
			pack.resterState = presterStateWireframe;
		}
		else {
			pack.resterState = presterStateSolidCullFront;
		}

		pack.batches.clear();
		batch.camPos = curCam.Position;
		batch.shaderType = SHADERSETDX10::TEXCOLOR;
		batch.proj = curCam.ProjMatrix;
		batch.view[0] = curCam.ViewMatrix;
		for(unsigned int i = 0; i < SHADERMAXSRV; ++i)
			batch.srvs[i] = NULL;

		batch.mesh = &skyBoxFace[0];
		batch.srvs[0] = resMen->getData(renderDesc.skyboxNorth)->srv;
		mat.scale(renderDesc.skyboxdist, renderDesc.skyboxdist, 
			renderDesc.skyboxdist*0.995f);
		batch.world = mat;
		pack.batches.push_back(batch);

		batch.mesh = &skyBoxFace[1];
		batch.srvs[0] = resMen->getData(renderDesc.skyboxSouth)->srv;
		mat.scale(renderDesc.skyboxdist, renderDesc.skyboxdist,
			renderDesc.skyboxdist*0.995f);
		batch.world = mat;
		pack.batches.push_back(batch);
		
		batch.mesh = &skyBoxFace[2];
		batch.srvs[0] = resMen->getData(renderDesc.skyboxEast)->srv;
		mat.scale(renderDesc.skyboxdist*0.995f, renderDesc.skyboxdist, 
			renderDesc.skyboxdist);
		batch.world = mat;
		pack.batches.push_back(batch);

		batch.mesh = &skyBoxFace[3];
		batch.srvs[0] = resMen->getData(renderDesc.skyboxWest)->srv;
		mat.scale(renderDesc.skyboxdist*0.995f, renderDesc.skyboxdist, 
			renderDesc.skyboxdist);
		batch.world = mat;
		pack.batches.push_back(batch);

		batch.mesh = &skyBoxFace[4];
		batch.srvs[0] = resMen->getData(renderDesc.skyboxTop)->srv;
		mat.scale(renderDesc.skyboxdist, renderDesc.skyboxdist*0.995f, 
			renderDesc.skyboxdist);
		batch.world = mat;
		pack.batches.push_back(batch);

		batch.mesh = &skyBoxFace[5];
		batch.srvs[0] = resMen->getData(renderDesc.skyboxBottom)->srv;
		mat.scale(renderDesc.skyboxdist, renderDesc.skyboxdist*0.995f, 
			renderDesc.skyboxdist);
		batch.world = mat;
		pack.batches.push_back(batch);

		batchPacks.push_back(pack);
	}
	
	pack.depthState = pdepthStencilStateDef;
	
	if(renderDesc.renderstate & RENDERDESCREPTION::APPLYWIREFRAME) {
		pack.resterState = presterStateWireframe;
	}
	else {
		pack.resterState = presterStateSolid;
	}
	
	cullBatches(curCam.Frustum);
	pack.batches.clear();
	for(unsigned int oi = 0; oi < visibleScene.size(); ++oi) {
		batch.camPos = curCam.Position;
		batch.color = fullScene[visibleScene[oi]]->Material->Kd;
		//batch.light.Color;
		//batch.light.Direction;
		//batch.light.Parameters;
		//batch.light.Position;
		//batch.light.Proj;
		//batch.light.View;
		batch.mesh = fullScene[visibleScene[oi]]->Mesh;
		batch.proj = curCam.ProjMatrix;
		for(unsigned int i = 0; i < SHADERMAXSRV; ++i) 
			batch.srvs[i] = NULL;
		if(fullScene[visibleScene[oi]]->Material->diffuseMap != NULL) {
			batch.shaderType = SHADERSETDX10::TEXCOLOR;
			batch.srvs[0] = fullScene[visibleScene[oi]]->Material->diffuseMap;
		}
		else {
			batch.shaderType = SHADERSETDX10::SOLIDCOLOR;
		}
		for(unsigned int i = 0; i < 6; ++i)
			batch.view[i] = curCam.ViewMatrix;
		batch.world = fullScene[visibleScene[oi]]->World;
			
		pack.batches.push_back(batch);
	}
	std::sort(pack.batches.begin(), pack.batches.end(), cmpRenderBatch);
	batchPacks.push_back(pack);
}

void RendererDX10::prepareLitBatches() {
	RENDERBATCH batch;
	RENDERBATCHPACK pack;
	int lt = 0;
	bool bump = false;
	bool spec = false;
	bool shadow = false;
	float col[] = {0.0f,0.0f,0.0f,0.0f};

	pack.rtv = resMen->getData(std::string("LightRenderTarget"))->rtv;
	pack.dsv = pbbD;
	pd3dDevice->ClearRenderTargetView(pack.rtv, col);
	pack.viewPort = curCam.ViewPort;
	pack.blendState = pblendStateAdd;
	pack.depthState = pdepthStencilStateEq;
	pack.resterState = presterStateSolid;
	
	cullBatches(curCam.Frustum);
	pack.batches.clear();
	if(renderDesc.renderstate & RENDERDESCREPTION::APPLYLIGHT) {
	for(unsigned int li = 0; li < lights.size(); ++li) {
		batch.camPos = curCam.Position;
		batch.color = lights[li]->Color;
		batch.light.Color = lights[li]->Color;
		batch.light.Direction = lights[li]->Direction;
		batch.light.Parameters = lights[li]->Parameters;
		batch.light.Position = lights[li]->Position;
		batch.light.Proj = lights[li]->Proj;
		batch.light.View = lights[li]->View[0];
		batch.proj = curCam.ProjMatrix;
		for(unsigned int i = 0; i < 6; ++i)
			batch.view[i] = curCam.ViewMatrix;

		for(unsigned int oi = 0; oi < visibleScene.size(); ++oi) {
			lt = 0; shadow = false; bump = false; spec = false;
			for(unsigned int i = 0; i < SHADERMAXSRV; ++i) 
				batch.srvs[i] = NULL;
			
			if(lights[li]->Type == LIGHTDX10::POINT) 
				lt = 1;
			else if(lights[li]->Type == LIGHTDX10::SPOT)
				lt = 2;
			if(renderDesc.renderstate & RENDERDESCREPTION::APPLYSHADOWS && 
				lights[li]->ShadowMapSize > 0 && lights[li]->Parameters.w == 1.0f) 
					shadow = true;
			if(fullScene[visibleScene[oi]]->Material->normalMap != NULL)
				bump = true;
			if(fullScene[visibleScene[oi]]->Material->specularMap != NULL) 
				spec = true;

			// point lighty
			if(lt == 1 && shadow && bump && spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHTSPECBUMPSHADOW;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->specularMap;
				batch.srvs[1] = fullScene[visibleScene[oi]]->Material->normalMap;
				batch.srvs[2] = lights[li]->pShadowMapSRV;
			}
			else if(lt == 1 && !shadow && bump && spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHTSPECBUMP;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->specularMap;
				batch.srvs[1] = fullScene[visibleScene[oi]]->Material->normalMap;
			}
			else if(lt == 1 && shadow && !bump && spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHTSPECSHADOW;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->specularMap;
				batch.srvs[1] = lights[li]->pShadowMapSRV;
			}
			else if(lt == 1 && !shadow && !bump && spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHTSPEC;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->specularMap;
			}
			else if(lt == 1 && shadow && bump && !spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHTBUMPSHADOW;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->normalMap;
				batch.srvs[1] = lights[li]->pShadowMapSRV;
			}
			else if(lt == 1 && !shadow && bump && !spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHTBUMP;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->normalMap;
			}
			else if(lt == 1 && shadow && !bump && !spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHTSHADOW;
				batch.srvs[0] = lights[li]->pShadowMapSRV;
			}
			else if(lt == 1 && !shadow && !bump && !spec) {
				batch.shaderType = SHADERSETDX10::POINTLIGHT;
			}

			// spot lighty
			if(lt == 2 && shadow && bump && spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHTSPECBUMPSHADOW;
				batch.srvs[0] = lights[li]->pShadowMapSRV;
				batch.srvs[1] = fullScene[visibleScene[oi]]->Material->specularMap;
				batch.srvs[2] = fullScene[visibleScene[oi]]->Material->normalMap;
			}
			else if(lt == 2 && !shadow && bump && spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHTSPECBUMP;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->specularMap;
				batch.srvs[1] = fullScene[visibleScene[oi]]->Material->normalMap;
			}
			else if(lt == 2 && shadow && !bump && spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHTSPECSHADOW;
				batch.srvs[0] = lights[li]->pShadowMapSRV;
				batch.srvs[1] = fullScene[visibleScene[oi]]->Material->specularMap;
			}
			else if(lt == 2 && !shadow && !bump && spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHTSPEC;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->specularMap;
			}
			else if(lt == 2 && shadow && bump && !spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHTBUMPSHADOW;
				batch.srvs[0] = lights[li]->pShadowMapSRV;
				batch.srvs[1] = fullScene[visibleScene[oi]]->Material->normalMap;
			}
			else if(lt == 2 && !shadow && bump && !spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHTBUMP;
				batch.srvs[0] = fullScene[visibleScene[oi]]->Material->normalMap;
			}
			else if(lt == 2 && shadow && !bump && !spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHTSHADOW;
				batch.srvs[0] = lights[li]->pShadowMapSRV;
			}
			else if(lt == 2 && !shadow && !bump && !spec) {
				batch.shaderType = SHADERSETDX10::SPOTLIGHT;
			}
			
			batch.world = fullScene[visibleScene[oi]]->World;
			batch.mesh = fullScene[visibleScene[oi]]->Mesh;

			pack.batches.push_back(batch);
		}
	}
	std::sort(pack.batches.begin(), pack.batches.end(), cmpRenderBatch);
	batchPacks.push_back(pack);
	}
}

void RendererDX10::prepareSSNormBatches() {
	RENDERBATCH batch;
	RENDERBATCHPACK pack;
	float col[] = {0.0f,0.0f,-1.0f,1.0f};

	pack.rtv = resMen->getData(std::string("SSNormals"))->rtv;
	pack.dsv = pbbD;
	pd3dDevice->ClearRenderTargetView(pack.rtv, col);
	pd3dDevice->ClearDepthStencilView(pack.dsv, D3D10_CLEAR_DEPTH, 1.0f, 0);
	pack.viewPort = curCam.ViewPort;
	pack.blendState = NULL;
	pack.depthState = pdepthStencilStateEq;
	pack.resterState = presterStateSolid;
	
	pack.batches.clear();
	for(unsigned int oi = 0; oi < visibleScene.size(); ++oi) {
		batch.camPos = curCam.Position;
		//batch.color = fullScene[visibleScene[oi]]->Material->Kd;
		//batch.light.Color;
		//batch.light.Direction;
		//batch.light.Parameters;
		//batch.light.Position;
		//batch.light.Proj;
		//batch.light.View;
		batch.mesh = fullScene[visibleScene[oi]]->Mesh;
		batch.proj = curCam.ProjMatrix;
		batch.shaderType = SHADERSETDX10::SSNORMALS;
		for(unsigned int i = 0; i < SHADERMAXSRV; ++i) 
			batch.srvs[i] = NULL;
		for(unsigned int i = 0; i < 6; ++i)
			batch.view[i] = curCam.ViewMatrix;
		batch.world = fullScene[visibleScene[oi]]->World;
			
		pack.batches.push_back(batch); 
	}
	std::sort(pack.batches.begin(), pack.batches.end(), cmpRenderBatch);
	batchPacks.push_back(pack);
}

void RendererDX10::prepareSSAOBatches() {
	RENDERBATCH batch;
	RENDERBATCHPACK pack;
	for(unsigned int i = 0; i < SHADERMAXSRV; ++i) 
		batch.srvs[i] = NULL;
	

	if(!(renderDesc.renderstate & RENDERDESCREPTION::APPLYSSAO)) {
		float cc[] = {1.0f,1.0f,1.0f,1.0f};
		pd3dDevice->ClearRenderTargetView(resMen->getData(std::string("AORenderTarget"))->rtv, cc); 
	}
	else {
		batch.shaderType = SHADERSETDX10::SSAO;
		batch.mesh = &fullScreenQuad;
		batch.srvs[0] = resMen->getData(std::string("SSNormals"))->srv;
		batch.srvs[1] = resMen->getData(std::string("SSRndNormals"))->srv;

		pack.rtv = resMen->getData(std::string("AORenderTarget"))->rtv;
		pack.dsv = NULL;
		pack.blendState = NULL;
		pack.depthState = pdepthStencilStateOffOff;
		pack.resterState = presterStateSolidCullFront;
		pack.viewPort = curCam.ViewPort;
		pack.batches.clear();
		pack.batches.push_back(batch);

		batchPacks.push_back(pack);
	}
}

void RendererDX10::prepareFinalMargeBatches() {
	RENDERBATCH batch;
	RENDERBATCHPACK pack;
	for(unsigned int i = 0; i < SHADERMAXSRV; ++i) 
		batch.srvs[i] = NULL;

	batch.shaderType = SHADERSETDX10::FINALMERGE;
	batch.mesh = &fullScreenQuad;
	batch.srvs[0] = resMen->getData(std::string("ColorRenderTarget"))->srv;
	batch.srvs[1] = resMen->getData(std::string("LightRenderTarget"))->srv;
	batch.srvs[2] = resMen->getData(std::string("AORenderTarget"))->srv;

	pack.rtv = pbbRT;
	pack.dsv = NULL;
	pack.blendState = NULL;
	pack.depthState = pdepthStencilStateOffOff;
	pack.resterState = presterStateSolidCullFront;
	pack.viewPort = curCam.ViewPort;
	pack.batches.clear();
	pack.batches.push_back(batch);

	batchPacks.push_back(pack);
}

void RendererDX10::prepareBatches() {
	batchPacks.clear();
	prepareColBatches();
	prepareSMBatches();
	prepareLitBatches();
	prepareSSNormBatches();
	prepareSSAOBatches();
	prepareFinalMargeBatches();
}

void RendererDX10::renderBatches() {
	unsigned int stride = sizeof(EVERTEX), zero = 0;
	float col[] = {0.5f,0.2f,0.7f,0.0f};

	for(unsigned int pi = 0; pi < batchPacks.size(); ++pi) {
		pd3dDevice->OMSetRenderTargets(1, &batchPacks[pi].rtv, batchPacks[pi].dsv);
		pd3dDevice->OMSetDepthStencilState(batchPacks[pi].depthState, 1);
		pd3dDevice->OMSetBlendState(batchPacks[pi].blendState, 0, 0xffffffff);
		pd3dDevice->RSSetState(batchPacks[pi].resterState);
		pd3dDevice->RSSetViewports(1, &batchPacks[pi].viewPort);

		for(unsigned int oi = 0; oi < batchPacks[pi].batches.size(); ++oi) {
			shaderMen->setCameraPosition(batchPacks[pi].batches[oi].camPos);
			shaderMen->setObjectColor(batchPacks[pi].batches[oi].color);
			shaderMen->setProjMatrix(batchPacks[pi].batches[oi].proj);
			shaderMen->setViewMatrix(batchPacks[pi].batches[oi].view[0]);
			for(unsigned int i = 0; i < 6; ++i)
				shaderMen->setCubeViewMatrix(i, batchPacks[pi].batches[oi].view[i]);
			shaderMen->setWorldMatrix(batchPacks[pi].batches[oi].world);
			shaderMen->setLightDesc(&batchPacks[pi].batches[oi].light);
			shaderMen->setTextureSRV(SHADERMAXSRV, batchPacks[pi].batches[oi].srvs);
			shaderMen->onBeginFrame(batchPacks[pi].batches[oi].shaderType);
			shaderMen->onRender(batchPacks[pi].batches[oi].shaderType);
			
			pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pd3dDevice->IASetVertexBuffers(0, 1, &batchPacks[pi].batches[oi].mesh->pdxvertexBuffer, 
				&stride, &zero);
			pd3dDevice->IASetIndexBuffer(batchPacks[pi].batches[oi].mesh->pdxindexBuffer, 
				DXGI_FORMAT_R32_UINT, 0);
			pd3dDevice->DrawIndexed(batchPacks[pi].batches[oi].mesh->indexCount, 
				batchPacks[pi].batches[oi].mesh->indexStart, 
				batchPacks[pi].batches[oi].mesh->vertexStart);

			shaderMen->onEndFrame();
		}
	}
}

void RendererDX10::postProcessBlur2D(UINT outWidth, UINT outHeight, 
	ID3D10ShaderResourceView* in, ID3D10RenderTargetView* out) {
	D3D10_VIEWPORT vp = curCam.ViewPort;
	vp.Height = outHeight;
	vp.Width = outWidth;

	pd3dDevice->OMSetRenderTargets(1, &out, NULL);
	pd3dDevice->RSSetViewports(1, &vp);
	shaderMen->setTextureSRV(1,&in);

	shaderMen->onBeginFrame(SHADERSETDX10::BLUR);
	pd3dDevice->RSSetState(presterStateSolidCullFront);
	pd3dDevice->OMSetDepthStencilState(pdepthStencilStateOffOff, 0);
	pd3dDevice->OMSetBlendState(NULL, 0, 0xffffffff);
	shaderMen->onRender(SHADERSETDX10::BLUR);
	unsigned int stride = sizeof(EVERTEX), offset = 0;
	pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDevice->IASetVertexBuffers(0,1,&resMen->getData(std::string("FullScreenQuadVB"))->buf, 
		&stride, &offset);
	pd3dDevice->Draw(6, 0);
	shaderMen->onEndFrame();
}

void RendererDX10::postProcessBlurCube(ID3D10ShaderResourceView* in, 
	ID3D10RenderTargetView* out) {
	pd3dDevice->OMSetRenderTargets(1, &out, NULL);
	pd3dDevice->RSSetViewports(1, &curCam.ViewPort);
	shaderMen->setTextureSRV(1,&in);

	shaderMen->onBeginFrame(SHADERSETDX10::BLURCUBE);
	pd3dDevice->RSSetState(presterStateSolidCullFront);
	pd3dDevice->OMSetDepthStencilState(pdepthStencilStateOffOff, 0);
	pd3dDevice->OMSetBlendState(NULL, 0, 0xffffffff);
	shaderMen->onRender(SHADERSETDX10::BLURCUBE);
	unsigned int stride = sizeof(EVERTEX), offset = 0;
	pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDevice->IASetVertexBuffers(0,1,&resMen->getData(std::string("FullScreenQuadVB"))->buf, 
		&stride, &offset);
	pd3dDevice->Draw(6, 0);
	shaderMen->onEndFrame();
}

const std::string RendererDX10::getUniqueName() {
	static int seed = 0;
	std::string ret("RT");
	char num[128] = {0};

	_itoa_s(seed++, num, 128, 10);

	ret += num;

	return std::string(ret);
}

const std::string RendererDX10::getShadowMapName(bool isCube, unsigned int smsize) {
	std::string ret("SM");
	char num[64] = {0};
	_itoa_s(smsize, num, 63, 10);
	ret += num;
	if(isCube) ret += "C";
	return ret;
}

void RendererDX10::render() {
	prepareBatches();
	renderBatches();
	pswapChain->Present(0,0);
}

void RendererDX10::add(RENDERABLE* me) {
	fullScene.push_back(me);
}

void RendererDX10::add(LIGHTDX10* lig) {
	lights.push_back(lig);
}

void RendererDX10::remAll() {
	fullScene.clear();
	visibleScene.clear();
}

void RendererDX10::rem(RENDERABLE* me) {
	fullScene.erase(std::find(fullScene.begin(), fullScene.end(), me));
}

void RendererDX10::rem(LIGHTDX10* lig) {
	lights.erase(std::find(lights.begin(), lights.end(), lig));
}

ResourceMenagerDX10* RendererDX10::getResourceMenager() {
	return resMen;
}

IDXGISwapChain* RendererDX10::getSwapChain() {
	return pswapChain;
}

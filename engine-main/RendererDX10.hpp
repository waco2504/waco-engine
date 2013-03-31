#pragma once

#ifndef _INC_RENDERERDX10_
#define _INC_RENDERERDX10_

#include "DXClass.hpp"
#include "Singleton.hpp"

#include "ShaderMenagerDX10.hpp"
#include "ResourceMenagerDX10.hpp"
#include "OBJLoader.hpp"

#include <map>
#include <deque>
#include <algorithm>
#include <vector>

#define MAXRENDERBATCHSRV 2

struct CAMERADX10 {
	D3D10_VIEWPORT ViewPort;
	EVECTOR4 Position;
	EMATRIX ProjMatrix;
	EMATRIX ViewMatrix;
	EFRUSTUM Frustum;
};

struct MESHDX10 {
	struct EVERTEX {
		EVECTOR3 position;
		EVECTOR3 normal;
		EVECTOR4 tangent;			// w - ma dodatkowe info
		EVECTOR3 texture0;
	};
	char meshName[32];
	unsigned int indexCount;
	unsigned int indexStart;
	unsigned int vertexStart;
	ID3D10Buffer* pdxindexBuffer;
	ID3D10Buffer* pdxvertexBuffer;
	EVECTOR3 BB[8];
};

struct MATERIALDX10 {
	char matName[32];
	EVECTOR4 Kd;
	ID3D10ShaderResourceView* diffuseMap;
	ID3D10ShaderResourceView* normalMap;
	ID3D10ShaderResourceView* specularMap;
};

struct RENDERABLE {
	EMATRIX World;
	MESHDX10* Mesh;
	MATERIALDX10* Material;
	EVECTOR3 AABB[8];
};

bool operator==(const RENDERABLE& l, const RENDERABLE& r);

struct LIGHTDX10 {
	/*	Parameters:
		x - inner cone angle, y - outter cone angle, 
		z - attenuation(zanik), w - rysuj cien(?) 
		shadowmapa swiatla niech bedzie 64,128,256,512,1024*/
	enum TYPE : int {
		UNKNOWN = 0x00,
		POINT = 0x01,
		SPOT = 0x02,
		DIRECTIONAL = 0x04
	};

	char lightName[32];
	TYPE Type;
	unsigned int ShadowMapSize;
	ID3D10RenderTargetView* pShadowMapRTV;
	ID3D10ShaderResourceView* pShadowMapSRV;
	ID3D10RenderTargetView* pShadowMapRTVArray[6];
	ID3D10ShaderResourceView* pShadowMapSRVArray[6];
	D3D10_VIEWPORT ViewPort;
	EMATRIX Proj;
	EMATRIX View[6];
	EFRUSTUM Frustum[6];
	EVECTOR4 Position;
	EVECTOR4 Color;
	EVECTOR4 Direction;
	EVECTOR4 Parameters;
};

bool operator==(const LIGHTDX10& l, const LIGHTDX10& r);

struct RENDERBATCH {
	EMATRIX proj;
	EMATRIX view[6];
	EMATRIX world;
	MESHDX10* mesh;
	SHADERSETDX10::TYPE shaderType;
	ShaderMenagerDX10::LIGHT_DESCREPTION light;
	EVECTOR4 color;
	EVECTOR4 camPos;
	ID3D10ShaderResourceView* srvs[MAXRENDERBATCHSRV];
};

bool operator==(const RENDERBATCH& l, const RENDERBATCH& r);

struct RENDERBATCHPACK {
	ID3D10RenderTargetView* rtv;
	ID3D10DepthStencilView* dsv;
	D3D10_VIEWPORT viewPort;
	ID3D10RasterizerState* resterState;
	ID3D10DepthStencilState* depthState;
	ID3D10BlendState* blendState;
	std::vector<RENDERBATCH> batches;
};

struct RENDERDESCREPTION {
	enum STATE : int {
		APPLYSSAO = 0x01,
		APPLYLIGHT = 0x02,
		APPLYCOLOR = 0x04,
		APPLYWIREFRAME = 0x08,
		APPLYSHADOWS = 0x10,
		APPLYONEPASSCUBESHADOWS = 0x20,
		APPLYSKYBOX = 0x40,
	};
	enum SHADOWQUALITY : int {
		POINT,
		//PCF3X3,
	};
	int renderstate;
	int shadowQuality;
	float skyboxdist;
	std::string skyboxEast;
	std::string skyboxWest;
	std::string skyboxNorth;
	std::string skyboxSouth;
	std::string skyboxTop;
	std::string skyboxBottom;
};

class RendererDX10 : public DXClass {
	ResourceMenagerDX10* resMen;
	ShaderMenagerDX10* shaderMen;

	IDXGISwapChain* pswapChain;
	
	ID3D10RenderTargetView* pbbRT;	// rt buffer
	ID3D10Texture2D* pbbDRes;
	ID3D10DepthStencilView* pbbD;	// depth buffer

	ID3D10DepthStencilState* pdepthStencilStateDef;
	ID3D10DepthStencilState* pdepthStencilStateEq;
	ID3D10DepthStencilState* pdepthStencilStateLessEq;
	ID3D10DepthStencilState* pdepthStencilStateEqOff;
	ID3D10DepthStencilState* pdepthStencilStateLessEqOff;
	ID3D10DepthStencilState* pdepthStencilStateOffOff;
	ID3D10BlendState* pblendStateAdd;
	ID3D10BlendState* pblendStateColorOff;
	ID3D10RasterizerState* presterStateSolid;
	ID3D10RasterizerState* presterStateSolidCullFront;
	ID3D10RasterizerState* presterStateSolidCullOff;
	ID3D10RasterizerState* presterStateWireframe; 

	MESHDX10 fullScreenQuad;
	MESHDX10 skyBoxFace[6];

	std::vector<RENDERABLE*> fullScene;
	std::vector<unsigned int> visibleScene;
	std::vector<LIGHTDX10*> lights;
	
	std::vector<RENDERBATCHPACK> batchPacks;
	
	CAMERADX10 curCam;
	RENDERDESCREPTION renderDesc;

	void cullBatches(const EFRUSTUM frustum);

	void initBackBufferRT();
	void initShadowDepthBuffer();
	void initDepthStencilState();
	void initBlendState();
	void initResterState();
	void initFullScreenQuad();
	void initSkyBox();

	void prepareSMBatches();
	void prepareColBatches();
	void prepareLitBatches();
	void prepareSSNormBatches();
	void prepareBatches();
	void renderBatches();

	void renderSSAO();	
	void postProcessBlur2D(UINT outWidth, UINT outHeight, 
		ID3D10ShaderResourceView* in, ID3D10RenderTargetView* out);
	// UWAGA narazie out musi miec rozmiar jak glowny RT
	void postProcessBlurCube(ID3D10ShaderResourceView* in, 
		ID3D10RenderTargetView* out);

	void finalMarge();

	const std::string getUniqueName();
	const std::string getShadowMapName(bool isCube, unsigned int smsize);
public:
	void init(HWND hWnd, UINT clientWidth, UINT clientHeight, 
		bool isFullscreen);

	void setDefaultRenderDescreption();
	void setRenderDescreption(const RENDERDESCREPTION& desc);
	void setCamera(const CAMERADX10 pcamera);
	
	void render();

	void add(RENDERABLE*);
	void add(LIGHTDX10*);

	void remAll();
	void rem(RENDERABLE*);
	void rem(LIGHTDX10*);


	ResourceMenagerDX10* getResourceMenager();

	IDXGISwapChain* getSwapChain();
};

#endif
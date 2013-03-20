#include "WindowFramework.hpp"
#include "InputSystemDX.hpp"
#include "RendererDX10.hpp"
#include "FactoryMeshDX10.hpp"
#include "ShaderMenagerDX10.hpp"
#include "Camera.hpp"

#include "Timer.hpp"

#include <d3dx10.h>

#define _WHIGHT 1080
#define _WWIDTH 1920

Window* g_win = NULL;
InputSystemDX* g_insys = NULL;
RendererDX10* g_ren = NULL;
ResourceMenagerDX10* g_res = NULL;
FactoryMeshDX10* g_fm = NULL;

CAMERADX10 renCam;
Camera* cam = NULL;
RENDERDESCREPTION renDesc;

bool ssao = true;
bool shadows = true;
bool lightson = true;

std::vector<RENDERABLE*> g_vscene;

LIGHTDX10 l1, l2;
Camera* l1Cam = NULL, *l2Cam = NULL;

Timer timer;

void init();
void loadScene();
void updateCam();

int WINAPI wWinMain(HINSTANCE wInst, HINSTANCE, LPWSTR, int) {
	init();
	loadScene();

	
	g_ren->add(&l2);
	g_ren->add(&l1);

	MATERIALDX10 defMat;
	defMat.diffuseMap = NULL;
	defMat.normalMap = NULL;
	strcpy_s(defMat.matName, "default");
	defMat.Kd = EVECTOR4(0.5f,0.3f,0.8f,1.0f);

	for(unsigned int i = 0; i < g_fm->getLoadedBatchSize(); ++i) {
		g_vscene.push_back(new RENDERABLE());
		g_vscene.back()->Material = g_fm->getLoadedBatch(i).Material;
		//g_vscene.back()->Material = &defMat;
		g_vscene.back()->Mesh = g_fm->getLoadedBatch(i).Mesh;
		g_vscene.back()->World.identity();
		g_vscene.back()->World.rotateY(3.1415f*0.3f);
		for(unsigned int v = 0; v < 8; ++v)
			g_vscene.back()->AABB[v] = g_vscene.back()->World * g_vscene.back()->Mesh->BB[v];
		g_ren->add(g_vscene[g_vscene.size()-1]);
	}

	float ang = 0.0f;
	while(g_win->isVisiable()) {
		g_insys->AcquireKeyboard();
		g_win->update();
		
		updateCam();

		ang += timer.getMilliSeconds() * 0.00005 * 3.1415f; 
		if(ang >= 6.283f) ang -= 6.283f;

		//for(unsigned int i = 0; i < g_vscene.size(); ++i) {
			//g_vscene[i]->World.identity();
			//g_vscene[i]->World.rotateY(ang);
		//}

		timer.start();

		g_ren->setCamera(renCam);
		g_ren->setRenderDescreption(renDesc);
		g_ren->render();

		timer.update();
	}
	g_insys->UnacquireKeyboard();


	for(unsigned int i = 0; i < g_vscene.size(); ++i) {
		delete g_vscene[i];
	}

	return 0;
}

void init() {
	g_win = WindowFactory::getSingleton()->createWindow("EEngine", _WWIDTH,_WHIGHT);
	
	g_insys = new InputSystemDX(g_win->getWindowInfo().hWnd);

	g_ren = new RendererDX10();
	g_ren->init(g_win->getWindowInfo().hWnd, _WWIDTH, _WHIGHT, true);
	
	g_res = g_ren->getResourceMenager();

	g_fm = new FactoryMeshDX10();
	g_fm->setDxDevice(g_ren->getDxDevice());
	g_fm->init(g_res);
	
	cam = new Camera(3.1415f * 0.40f, g_win->getWindowInfo().client_width, 
		g_win->getWindowInfo().client_height, 0.1f, 10000.0f, EVECTOR3(0.0f,800.0f,0.0f),
		EVECTOR3(1.0f,0.0f,0.0f), EVECTOR3(0.0f,1.0f,0.0f)); 

	renDesc.renderstate = RENDERDESCREPTION::APPLYLIGHT | 
		RENDERDESCREPTION::APPLYSSAO | RENDERDESCREPTION::APPLYSKYBOX | 
		RENDERDESCREPTION::APPLYONEPASSCUBESHADOWS;
	renDesc.shadowQuality = RENDERDESCREPTION::POINT;
	renDesc.skyboxdist = 100.0f;
	renDesc.skyboxEast = std::string("alpine_east");
	renDesc.skyboxWest = std::string("alpine_west");
	renDesc.skyboxNorth = std::string("alpine_north");
	renDesc.skyboxSouth = std::string("alpine_south");
	renDesc.skyboxTop = std::string("alpine_up");
	renDesc.skyboxBottom = std::string("alpine_down");
}

void loadScene() {
	g_fm->loadMeshesFromOBJ(".\\..\\Debug\\Sponza.obj");
	//g_fm->saveMeshesInMeshFile("C:\\Users\\waco\\Desktop\\engine\\Debug\\spmesh");
	//if(!g_fm->loadMeshesFromMeshFile("C:\\Users\\waco\\Desktop\\engine\\Debug\\sponzamesh")) MessageBox(NULL, "FAILED", "", MB_OK);
	
	g_res->loadTexture2DFromFile(renDesc.skyboxEast, 
		".\\..\\Debug\\skybox\\alpine_east.bmp");
	g_res->loadTexture2DFromFile(renDesc.skyboxWest, 
		".\\..\\Debug\\skybox\\alpine_west.bmp");
	g_res->loadTexture2DFromFile(renDesc.skyboxNorth, 
		".\\..\\Debug\\skybox\\alpine_north.bmp");
	g_res->loadTexture2DFromFile(renDesc.skyboxSouth, 
		".\\..\\Debug\\skybox\\alpine_south.bmp");
	g_res->loadTexture2DFromFile(renDesc.skyboxTop, 
		".\\..\\Debug\\skybox\\alpine_up.bmp");
	g_res->loadTexture2DFromFile(renDesc.skyboxBottom, 
		".\\..\\Debug\\skybox\\alpine_down.bmp");

	// swiatlo punktowe
	unsigned int l1sms = 512;
	g_res->createTextureCube(std::string("PointLight1SM"), l1sms, l1sms, 1, 
		DXGI_FORMAT_R32G32_FLOAT, D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE);

	ZeroMemory(&l1, sizeof(LIGHTDX10));
	strcpy_s(l1.lightName, 32, "PointLight1");
	l1.Type = LIGHTDX10::POINT;
	l1.Color = EVECTOR4(0.7f,0.7f,0.7f,1.0f);
	l1.Position = EVECTOR4(0.0f,600.0f,0.0f,1.0f);
	l1.Direction = EVECTOR4(1.0f,0.0f,0.0f,1.0f);
	l1.Parameters = EVECTOR4(0.0f,0.0f,0.0f,1.0f);
	l1.ShadowMapSize = l1sms;
	
	l1Cam = new Camera(3.1415f/2.0f, l1sms, l1sms, 1.0f, 10000.0f, l1.Position, 
		l1.Direction, EVECTOR3(0.0f,1.0f,0.0f));
	l1.Proj = l1Cam->getProjectionMatrix();
	for(unsigned int i = 0; i < 6; ++i) {
		l1.View[i] = l1Cam->getCubeViewMatrix(i);
		l1.Frustum[i] = l1Cam->getCubeFrustum(i);
	}
	l1.ViewPort = *(const D3D10_VIEWPORT*)l1Cam->getViewPort();

	l1.pShadowMapRTV = g_res->getData(std::string("PointLight1SM"))->rtv;
	l1.pShadowMapSRV = g_res->getData(std::string("PointLight1SM"))->srv;
	for(unsigned int i = 0; i < 6; ++i) {
		l1.pShadowMapRTVArray[i] = g_res->getData(std::string("PointLight1SM"))->rtvArray[i];
		l1.pShadowMapSRVArray[i] = g_res->getData(std::string("PointLight1SM"))->srvArray[i];
	}
	////////////////////////////////////////

	// swiatlo halogen
	unsigned int l2sms = 512;
	g_res->createTexture2D(std::string("SpotLight1SM"), l2sms, l2sms, 1, 
		DXGI_FORMAT_R32G32_FLOAT, D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE);

	ZeroMemory(&l2, sizeof(LIGHTDX10));
	strcpy_s(l2.lightName, 32, "SpotLight1");
	l2.Type = LIGHTDX10::SPOT;
	l2.Color = EVECTOR4(0.7f,0.7f,0.7f,1.0f);
	l2.Position = EVECTOR4(-500.0f,1600.0f,300.0f,1.0f);
	EVECTOR4 d(1.0f,-2.0f,0.0f,1.0f); d.normalize();
	l2.Direction = d;
	l2.Parameters = EVECTOR4((3.1415f/4.0f)*0.90f,(3.1415f/4.0f)*1.10f,0.0f,1.0f);
	l2.ShadowMapSize = l2sms;
	
	l2Cam = new Camera((3.1415f/4.0f)*1.10f, l2sms, l2sms, 1.0f, 10000.0f, l2.Position, 
		l2.Direction, EVECTOR3(0.0f,1.0f,0.0f));
	l2.Proj = l2Cam->getProjectionMatrix();
	l2.View[0] = l2Cam->getViewMatrix();
	l2.Frustum[0] = l2Cam->getFrustum();
	l2.ViewPort = *(const D3D10_VIEWPORT*)l2Cam->getViewPort();
	l2.pShadowMapRTV = g_res->getData(std::string("SpotLight1SM"))->rtv;
	l2.pShadowMapSRV = g_res->getData(std::string("SpotLight1SM"))->srv;
	////////////////////////////////////////
}

void updateCam() {
	g_insys->refresh();

	static float counter = 0.0f;

	if(g_insys->isPressed(DIK_ESCAPE)) { 
		g_win->showWindow(false);
		return;
	}

	unsigned long int t = timer.getMilliSeconds();
	counter -= t;

	if(g_insys->isPressed(DIK_W)) cam->moveForward(0.4f * t);
	if(g_insys->isPressed(DIK_S)) cam->moveForward(-0.4f * t);
	if(g_insys->isPressed(DIK_A)) cam->moveLeft(0.4f * t);
	if(g_insys->isPressed(DIK_D)) cam->moveLeft(-0.4f * t);
	if(g_insys->isPressed(DIK_Q)) cam->moveUp(-0.4f * t);
	if(g_insys->isPressed(DIK_E)) cam->moveUp(0.4f * t);
	if(g_insys->isPressed(DIK_J)) cam->rotateYaw(-0.004f * t);
	if(g_insys->isPressed(DIK_L)) cam->rotateYaw(0.004f * t);
	if(g_insys->isPressed(DIK_I)) cam->rotatePitch(-0.004f * t);
	if(g_insys->isPressed(DIK_K)) cam->rotatePitch(0.004f * t);

	//l2.Direction = cam->getDirection();
	///l2.Frustum[0] = cam->getFrustum();
	//l2.Position = cam->getPosition();
	//l2.Proj = cam->getProjectionMatrix();
	//l2.View[0] = cam->getViewMatrix();

	/*if(g_insys->isPressed(DIK_UP)) {
		l1.Position += EVECTOR4(0.0f,0.0f,0.4f*t,0.0f);
	}
	if(g_insys->isPressed(DIK_DOWN)) {
		l1.Position += EVECTOR4(0.0f,0.0f,-0.4f*t,0.0f);
	}
	if(g_insys->isPressed(DIK_LEFT)) {
		l1.Position += EVECTOR4(0.4f*t,0.0f,0.0f,0.0f);
	}
	if(g_insys->isPressed(DIK_RIGHT)) {
		l1.Position += EVECTOR4(-0.4f*t,0.0f,0.0f,0.0f);
	}
	if(g_insys->isPressed(DIK_NUMPAD1)) {
		l1.Position += EVECTOR4(0.0f,0.4f*t,0.0f,0.0f);
	}
	if(g_insys->isPressed(DIK_NUMPAD2)) {
		l1.Position += EVECTOR4(0.0f,-0.4f*t,0.0f,0.0f);
	}*/

	if(g_insys->isPressed(DIK_F1) && counter <= 0.0f) { 
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYSSAO) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYSSAO;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYSSAO;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F2) && counter <= 0.0f) {
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYSHADOWS) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYSHADOWS;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYSHADOWS;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F3) && counter <= 0.0f) {
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYLIGHT) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYLIGHT;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYLIGHT;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F4) && counter <= 0.0f) {
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYCOLOR) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYCOLOR;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYCOLOR;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F5) && counter <= 0.0f) {
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYONEPASSCUBESHADOWS) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYONEPASSCUBESHADOWS;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYONEPASSCUBESHADOWS;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F6) && counter <= 0.0f) {
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYSKYBOX) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYSKYBOX;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYSKYBOX;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F7) && counter <= 0.0f) {
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYWIREFRAME) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYWIREFRAME;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYWIREFRAME;
		counter = 400.0f;
	}

	renCam.ViewPort = *(const D3D10_VIEWPORT*)cam->getViewPort();
	renCam.ProjMatrix = cam->getProjectionMatrix();
	renCam.ViewMatrix = cam->getViewMatrix();
	renCam.Position = cam->getPosition();
	renCam.Frustum = cam->getFrustum();
}
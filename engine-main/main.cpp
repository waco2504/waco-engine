#include "WindowFramework.hpp"
#include "InputSystemDX.hpp"
#include "RendererDX10.hpp"
#include "FactoryMeshDX10.hpp"
#include "ShaderMenagerDX10.hpp"
#include "Camera.hpp"
#include "ConfigMenager.hpp"

#include "Timer.hpp"

#include <fstream>

Window* g_win = NULL;
InputSystemDX* g_insys = NULL;
RendererDX10* g_ren = NULL;
ResourceMenagerDX10* g_res = NULL;
FactoryMeshDX10* g_fm = NULL;
ConfigFile* g_cfg = NULL;

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

	//g_ren->add(&l1);
	g_ren->add(&l2);

	for(unsigned int i = 0; i < g_fm->getLoadedBatchSize(); ++i) {
		g_vscene.push_back(new RENDERABLE());
		g_vscene.back()->Material = g_fm->getLoadedBatch(i).Material;
		g_vscene.back()->Mesh = g_fm->getLoadedBatch(i).Mesh;
		g_vscene.back()->World.identity();
		//g_vscene.back()->World.rotateY(3.1415f*0.3f);
		for(unsigned int v = 0; v < 8; ++v)
			g_vscene.back()->AABB[v] = g_vscene.back()->World * g_vscene.back()->Mesh->BB[v];
		g_ren->add(g_vscene[g_vscene.size()-1]);
	}

	while(g_win->isVisiable()) {
		g_insys->AcquireKeyboard();
		g_win->update();

		updateCam();
		
		timer.start();

		g_ren->setCamera(renCam);
		g_ren->setRenderDescreption(renDesc);
		g_ren->render();

		timer.update();
	}
	g_insys->UnacquireKeyboard();
	timer.stop();
	

	for(unsigned int i = 0; i < g_vscene.size(); ++i) {
		delete g_vscene[i];
	}

	WindowFactory::getSingleton()->deleteWindow(g_win);
	return 0;
}

void init() {
	g_cfg = new ConfigFile();
	if(g_cfg->open(std::string("engine.cfg"))) exit(1);

	g_win = WindowFactory::getSingleton()->createWindow("EEngine", 
		g_cfg->readConfigI("resolution-x"),
		g_cfg->readConfigI("resolution-y"));

	g_insys = new InputSystemDX(g_win->getWindowInfo().hWnd);

	g_ren = new RendererDX10();
	g_ren->init(g_win->getWindowInfo().hWnd,
		g_cfg->readConfigI("resolution-x"),
		g_cfg->readConfigI("resolution-y"), false);

	g_res = g_ren->getResourceMenager();

	g_fm = new FactoryMeshDX10();
	g_fm->setDxDevice(g_ren->getDxDevice());
	g_fm->init(g_res);
	
	cam = new Camera(3.1415f * 0.40f, g_win->getWindowInfo().client_width, 
		g_win->getWindowInfo().client_height, 0.1f, 10000.0f, EVECTOR3(0.0f,800.0f,0.0f),
		EVECTOR3(1.0f,0.0f,0.0f), EVECTOR3(0.0f,1.0f,0.0f)); 

	renDesc.renderstate = RENDERDESCREPTION::APPLYLIGHT | RENDERDESCREPTION::APPLYCOLOR |
		RENDERDESCREPTION::APPLYSKYBOX | RENDERDESCREPTION::APPLYONEPASSCUBESHADOWS | 
		RENDERDESCREPTION::APPLYSHADOWS;
	renDesc.shadowQuality = RENDERDESCREPTION::POINT;
	renDesc.skyboxdist = 100.0f;
	renDesc.skyboxEast = std::string("east");
	renDesc.skyboxWest = std::string("west");
	renDesc.skyboxNorth = std::string("north");
	renDesc.skyboxSouth = std::string("south");
	renDesc.skyboxTop = std::string("up");
	renDesc.skyboxBottom = std::string("down");
}

void loadScene() {
	g_fm->loadMeshesFromOBJ(g_cfg->readConfigS("model-sponza-path"));

	g_res->loadTexture2DFromFile(renDesc.skyboxEast, 
		g_cfg->readConfigS("skybox-east-path").c_str());
	g_res->loadTexture2DFromFile(renDesc.skyboxWest, 
		g_cfg->readConfigS("skybox-west-path").c_str());
	g_res->loadTexture2DFromFile(renDesc.skyboxNorth, 
		g_cfg->readConfigS("skybox-north-path").c_str());
	g_res->loadTexture2DFromFile(renDesc.skyboxSouth, 
		g_cfg->readConfigS("skybox-south-path").c_str());
	g_res->loadTexture2DFromFile(renDesc.skyboxTop, 
		g_cfg->readConfigS("skybox-up-path").c_str());
	g_res->loadTexture2DFromFile(renDesc.skyboxBottom, 
		g_cfg->readConfigS("skybox-down-path").c_str());

	// swiatlo punktowe
	unsigned int l1sms = 1024;
	g_res->createTextureCube(std::string("PointLight1SM"), l1sms, l1sms, 1, 
		DXGI_FORMAT_R32G32_FLOAT, D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE);

	ZeroMemory(&l1, sizeof(LIGHTDX10));
	l1.lightName = "PointLight1";
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
	unsigned int l2sms = 1024;
	g_res->createTexture2D(std::string("SpotLight1SM"), l2sms, l2sms, 1, 
		DXGI_FORMAT_R32G32_FLOAT, D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE);

	ZeroMemory(&l2, sizeof(LIGHTDX10));
	l2.lightName = "SpotLight1";
	l2.Type = LIGHTDX10::SPOT;
	l2.Color = EVECTOR4(1.0f,1.0f,1.0f,1.0f);
	l2.Position = EVECTOR4(-1004.4f, 1554.8f, -116.439f, 1.0f);
	EVECTOR4 d(0.715435f, -0.670672f, 0.195837f, 1.0f); //d.normalize();
	l2.Direction = d;
	float l1ang = 3.1415f/3.0f;
	l2.Parameters = EVECTOR4(l1ang*0.90f,l1ang,0.0f,1.0f);
	l2.ShadowMapSize = l2sms;
	
	l2Cam = new Camera(l1ang*1.10f, l2sms, l2sms, 1.0f, 5000.0f, l2.Position, 
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
	static float counter = 0.0f;

	g_insys->refresh();

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
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYSKYBOX) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYSKYBOX;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYSKYBOX;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F6) && counter <= 0.0f) {
		if(renDesc.renderstate & RENDERDESCREPTION::APPLYWIREFRAME) 
			renDesc.renderstate -= RENDERDESCREPTION::APPLYWIREFRAME;
		else 
			renDesc.renderstate += RENDERDESCREPTION::APPLYWIREFRAME;
		counter = 400.0f;
	}
	if(g_insys->isPressed(DIK_F9) && counter <= 0.0f) {
		std::ofstream ofile("camera.txt");
		ofile << cam->getPosition().x << " " << cam->getPosition().y << " " << cam->getPosition().z << std::endl;
		ofile << cam->getDirection().x << " " << cam->getDirection().y << " " << cam->getDirection().z << std::endl;
		ofile.close();
		MessageBox(NULL, "Zapisano stan kamery do camera.txt", "", MB_OK);
		counter = 400.0f;
	}

	renCam.ViewPort = *(const D3D10_VIEWPORT*)cam->getViewPort();
	renCam.ProjMatrix = cam->getProjectionMatrix();
	renCam.ViewMatrix = cam->getViewMatrix();
	renCam.Position = cam->getPosition();
	renCam.Frustum = cam->getFrustum();
}
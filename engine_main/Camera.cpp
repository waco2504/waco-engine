#include "Camera.hpp"


void Camera::updateViewMatrix() {
	EVECTOR3 at = pos + dir;
	viewMatrix.lookAtLH(pos, at, up);
	// +x
	cubeViewMatrix[0].lookAtLH(pos, pos+EVECTOR3(1.0f,0.0f,0.0f), EVECTOR3(0.0f,1.0f,0.0f));
	// -x
	cubeViewMatrix[1].lookAtLH(pos, pos+EVECTOR3(-1.0f,0.0f,0.0f), EVECTOR3(0.0f,1.0f,0.0f));
	// +y
	cubeViewMatrix[2].lookAtLH(pos, pos+EVECTOR3(0.0f,1.0f,0.0f), EVECTOR3(0.0f,0.0f,-1.0f));
	// -y
	cubeViewMatrix[3].lookAtLH(pos, pos+EVECTOR3(0.0f,-1.0f,0.0f), EVECTOR3(0.0f,0.0f,1.0f));
	// +z
	cubeViewMatrix[4].lookAtLH(pos, pos+EVECTOR3(0.0f,0.0f,1.0f), EVECTOR3(0.0f,1.0f,0.0f));
	// -z
	cubeViewMatrix[5].lookAtLH(pos, pos+EVECTOR3(0.0f,0.0f,-1.0f), EVECTOR3(0.0f,1.0f,0.0f));
	
	updateFrustum();
}

void Camera::updateProjMatrix() {
	projMatrix.identity();
	projMatrix.perspectiveFovLH(fOVA, 
		(float)viewPort.Width/(float)viewPort.Height, zNear, zFar);

	updateFrustum();
}

void Camera::updateFrustum() {
	frustum.make(fOVA, viewPort.Width/viewPort.Height, zNear, zFar, pos, dir, up);
	
	cubeFrustum[0].make(fOVA, 1.0f, zNear, zFar, pos, EVECTOR3(1.0f,0.0f,0.0f), EVECTOR3(0.0f,1.0f,0.0f));
	cubeFrustum[1].make(fOVA, 1.0f, zNear, zFar, pos, EVECTOR3(-1.0f,0.0f,0.0f), EVECTOR3(0.0f,1.0f,0.0f));
	cubeFrustum[2].make(fOVA, 1.0f, zNear, zFar, pos, EVECTOR3(0.0f,1.0f,0.0f), EVECTOR3(0.0f,0.0f,-1.0f));
	cubeFrustum[3].make(fOVA, 1.0f, zNear, zFar, pos, EVECTOR3(0.0f,-1.0f,0.0f), EVECTOR3(0.0f,0.0f,1.0f));
	cubeFrustum[4].make(fOVA, 1.0f, zNear, zFar, pos, EVECTOR3(0.0f,0.0f,1.0f), EVECTOR3(0.0f,1.0f,0.0f));
	cubeFrustum[5].make(fOVA, 1.0f, zNear, zFar, pos, EVECTOR3(0.0f,0.0f,-1.0f), EVECTOR3(0.0f,1.0f,0.0f));
}

Camera::Camera(float fova, unsigned int screenWidth, unsigned int screenHight,  
		float zn, float zf, const EVECTOR3& position, 
		const EVECTOR3& direction, const EVECTOR3& up) : pos(position), up(up), dir(direction) {
	viewPort.Height = screenHight;
	viewPort.Width = screenWidth;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	fOVA = fova;
	zNear = zn;
	zFar = zf;

	dir.normalize();

	updateProjMatrix();
	updateViewMatrix();
}

const EMATRIX& Camera::getViewMatrix() const {
	return viewMatrix;
}

const EMATRIX& Camera::getCubeViewMatrix(unsigned int idx) const {
	return cubeViewMatrix[idx];
}

const EMATRIX& Camera::getProjectionMatrix() const {
	return projMatrix;
}

const EVECTOR3& Camera::getPosition() const {
	return pos;
}

const EVECTOR3& Camera::getDirection() const {
	return dir;
}

const Camera::VIEWPORT* Camera::getViewPort() const {
	return &viewPort;
}

const EFRUSTUM Camera::getFrustum() const {
	return frustum;
}

const EFRUSTUM Camera::getCubeFrustum(unsigned int i) const {
	return cubeFrustum[i];
}

void Camera::setPosition(const EVECTOR3& p) {
	pos = p;
	updateViewMatrix();
}

void Camera::setDirection(const EVECTOR3& d) {
	dir = d;
	updateViewMatrix();
}

void Camera::setUp(const EVECTOR3& u) {
	up = u;
	updateViewMatrix();
}

void Camera::setFieldOfView(float f) {
	fOVA = f;
	updateProjMatrix();
}

void Camera::setScreenWidthHeight(unsigned int w, unsigned int h) {
	viewPort.Width = w;
	viewPort.Height = h;
	updateProjMatrix();
}

void Camera::translate(float x, float y, float z) {
	pos[0] += x;
	pos[1] += y;
	pos[2] += z;
	updateViewMatrix();
}

void Camera::moveForward(float x) {
	pos += dir * x;
	updateViewMatrix();
}

void Camera::moveLeft(float x) {
	EVECTOR3 ldir;
	ldir = dir.crossProduct(up);
	ldir.normalize();
	ldir *= x;
	pos += ldir;
	updateViewMatrix();
}

void Camera::moveUp(float x) {
	up.normalize();
	pos += up * x;
	updateViewMatrix();
}

void Camera::rotateYaw(float _ang) {
	EMATRIX mat;
	mat.rotateY(_ang);

	dir = mat * dir;
	dir.normalize();

	updateViewMatrix();
}

void Camera::rotatePitch(float _ang) {
	EMATRIX mat;
	EVECTOR3 axis;
	axis = dir.crossProduct(up);

	mat.rotateAxis(_ang, axis);

	dir = mat * dir;
	dir.normalize();

	updateViewMatrix();
}
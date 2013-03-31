#pragma once

#ifndef _INC_CAMERA_
#define _INC_CAMERA_

#include "Math.hpp"

class Camera {
public:
	struct VIEWPORT {
		int TopLeftX;
		int TopLeftY;
		unsigned int Width;
		unsigned int Height;
		float MinDepth;
		float MaxDepth;
	};
private:
	VIEWPORT viewPort;

	float fOVA;
	float zNear;
	float zFar;
	EVECTOR3 pos;
	EVECTOR3 dir;
	EVECTOR3 up;

	EFRUSTUM frustum;
	EFRUSTUM cubeFrustum[6];

	EMATRIX projMatrix;
	EMATRIX viewMatrix;
	EMATRIX cubeViewMatrix[6];

	void updateViewMatrix();
	void updateProjMatrix();
	void updateFrustum();
public:
	Camera(float fova, unsigned int screenWidth, unsigned int screenHight, 
		float znear, float zfar, const EVECTOR3& position, 
		const EVECTOR3& direction, const EVECTOR3& up);
	const EMATRIX& getViewMatrix() const;
	const EMATRIX& getCubeViewMatrix(unsigned int idx) const;
	const EMATRIX& getProjectionMatrix() const;
	const EVECTOR3& getPosition() const;
	const EVECTOR3& getDirection() const;
	const VIEWPORT* getViewPort() const;
	const EFRUSTUM getFrustum() const;
	const EFRUSTUM getCubeFrustum(unsigned int idx) const;

	void setPosition(const EVECTOR3& pos);
	void setDirection(const EVECTOR3& dir);
	void setUp(const EVECTOR3& up);
	void setFieldOfView(float fova);
	void setScreenWidthHeight(unsigned int sWidth, unsigned int sHeight);

	// zestaw funkcji manipuluj¹cych kamer¹
	///
	void translate(float x, float y, float z);

	void moveForward(float x);
	void moveLeft(float x);
	void moveUp(float x);
	void rotateYaw(float angle);
	void rotatePitch(float angle);
};

#endif

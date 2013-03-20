#include "Math.hpp"

#include <math.h>
#include <iostream>

EVECTOR::EVECTOR(const EVECTOR3& _v) {
	x = _v[0];
	y = _v[1];
	z = _v[2];
	w = 1.0f;
}

EVECTOR::EVECTOR(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
}

float& EVECTOR::operator[](int _idx) {
	if(_idx == 0) return x;
	else if(_idx == 1) return y;
	else if(_idx == 2) return z;
	else return w;
}

float EVECTOR::operator[](int _idx) const {
	if(_idx == 0) return x;
	else if(_idx == 1) return y;
	else if(_idx == 2) return z;
	else return w;
}

const EVECTOR EVECTOR::operator+(const EVECTOR& r) const {
	return EVECTOR(x+r.x,y+r.y,z+r.z,w+r.w);
}

const EVECTOR EVECTOR::operator-(const EVECTOR& r) const {
	return EVECTOR(x-r.x,y-r.y,z-r.z,w-r.w);
}

bool EVECTOR::operator==(const EVECTOR& r) const {
	return x == r.x && y == r.y && z == r.z && w == r.w;
}

void EVECTOR::operator/=(const float _r) {
	x /= _r;
	y /= _r;
	z /= _r;
	w /= _r;
}

void EVECTOR::operator+=(const EVECTOR& _r) {
	x += _r.x;
	y += _r.y;
	z += _r.z;
	w += _r.w;
}

void EVECTOR::normalize() {
	//float _l = x*x + y*y + z*z + w*w;
	
	//if(_l != 0.0f) {
	//	_l = sqrt(_l);

	//	x /= _l;
	//	y /= _l;
	//	z /= _l;
	//	w /= _l;
	//}

	float _l = x*x + y*y + z*z;
	
	if(_l != 0.0f) {
		_l = sqrt(_l);

		x /= _l;
		y /= _l;
		z /= _l;
	}
}

float EVECTOR::length() const {
	return sqrt(x*x + y*y + z*z);
}

EVECTOR::operator EVECTOR3() {
	return EVECTOR3(x,y,z);
}

EVECTOR3::EVECTOR3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {

}

EVECTOR3::EVECTOR3(float* _v) : x(_v[0]), y(_v[1]), z(_v[2]) {

}

float& EVECTOR3::operator[](int _idx) {
	if(_idx == 0) return x;
	else if(_idx == 1) return y;
	else return z;
}

float EVECTOR3::operator[](int _idx) const {
	if(_idx == 0) return x;
	else if(_idx == 1) return y;
	else return z;
}

void EVECTOR3::normalize() {
	float n = 1.0f / sqrtf(x*x + y*y + z*z);

	x *= n;
	y *= n;
	z *= n;
}

const float EVECTOR3::length() const {
	return sqrtf(x*x + y*y + z*z);
}

const float EVECTOR3::dotProduct(const EVECTOR3& _r) const {
	return x*_r[0] + y*_r[1] + z*_r[2];
}

const EVECTOR3 EVECTOR3::crossProduct(const EVECTOR3& _r) const {
	return EVECTOR3(
		y * _r[2] - z * _r[1],
		z * _r[0] - x * _r[2],
		x * _r[1] - y * _r[0] );
}

const EVECTOR3 EVECTOR3::operator+(const EVECTOR3& _r) const {
	return EVECTOR3(x+_r[0], y+_r[1], z+_r[2]);
}

const EVECTOR3 EVECTOR3::operator-(const EVECTOR3& _r) const {
	return EVECTOR3(x-_r[0], y-_r[1], z-_r[2]);
}

const EVECTOR3 EVECTOR3::operator*(const EVECTOR3& _r) const {
	return EVECTOR3(x*_r[0], y*_r[1], z*_r[2]);
}

const EVECTOR3 EVECTOR3::operator*(const float r) const {
	return EVECTOR3(x*r, y*r, z*r);
}

const EVECTOR3 EVECTOR3::operator-() const {
	return EVECTOR3(-x,-y,-z);
}

void EVECTOR3::operator+=(const EVECTOR3& _r) {
	x+=_r[0];
	y+=_r[1];
	z+=_r[2];
}

void EVECTOR3::operator*=(const float _r) {
	x *= _r;
	y *= _r;
	z *= _r;
}

void EVECTOR3::operator/=(const float _r) {
	x /= _r;
	y /= _r;
	z /= _r;
}

bool EVECTOR3::operator==(const EVECTOR3& r) const {
	return x == r.x && y == r.y && z == r.z;
}

EMATRIX::EMATRIX() { 
	for(unsigned char i = 0; i < 16; ++i) mat[i] = 0.0f;
}

EMATRIX::EMATRIX(float* _m) {
	for(unsigned char i = 0; i < 16; ++i) mat[i] = _m[i];
}

EMATRIX::EMATRIX(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13, 
				float m20, float m21, float m22, float m23, 
				float m30, float m31, float m32, float m33 ) {
					m[0][0] = m00;
					m[0][1] = m01;
					m[0][2] = m02;
					m[0][3] = m03;
					m[1][0] = m10;
					m[1][1] = m11;
					m[1][2] = m12;
					m[1][3] = m13;
					m[2][0] = m20;
					m[2][1] = m21;
					m[2][2] = m22;
					m[2][3] = m23;
					m[3][0] = m30;
					m[3][1] = m31;
					m[3][2] = m32;
					m[3][3] = m33;
}

void EMATRIX::identity() {
	for(unsigned char i = 0; i < 16; ++i) mat[i] = 0.0f;
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
}

void EMATRIX::transpose() {
	float temp[4][4] = { {0.0f}, };
	for(unsigned char i = 0; i < 4; ++i) {
		for(unsigned char j = 0; j < 4; ++j) {
			temp[i][j] = m[i][j];
		}
	}

	for(unsigned char i = 0; i < 4; ++i) {
		for(unsigned char j = 0; j < 4; ++j) {
			m[j][i] = temp[i][j];
		}
	}
}

float EMATRIX::determinant() {
	float det = 0.0f;

	/*det += m[0][0] * (	m[1][1]*m[2][2]*m[3][3] + m[1][2]*m[2][3]*m[3][1] + m[1][3]*m[2][1]*m[3][2]
					   -m[1][3]*m[2][2]*m[3][1] - m[1][2]*m[2][1]*m[3][3] - m[1][1]*m[2][3]*m[3][2]	);

	det += m[0][1] * (	m[1][0]*m[2][2]*m[3][3] + m[1][2]*m[2][3]*m[3][0] + m[1][3]*m[2][0]*m[3][2]
					   -m[1][3]*m[2][2]*m[3][0] - m[1][2]*m[2][0]*m[3][3] - m[1][0]*m[2][3]*m[3][2]	);

	det += m[0][2] * (	m[1][0]*m[2][1]*m[3][3] + m[1][1]*m[2][3]*m[3][0] + m[1][3]*m[2][0]*m[3][1]
					   -m[1][3]*m[2][1]*m[3][0] - m[1][1]*m[2][0]*m[3][3] - m[1][0]*m[2][3]*m[3][1]	);

	det += m[0][3] * (	m[1][0]*m[2][1]*m[3][2] + m[1][1]*m[2][2]*m[3][0] + m[1][2]*m[2][0]*m[3][1]
					   -m[1][2]*m[2][1]*m[3][0] - m[1][1]*m[2][0]*m[3][2] - m[1][0]*m[2][2]*m[3][1]	);*/

	det =	((*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1)) * 
			((*this)(2, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(2, 3)) - 
			((*this)(0, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(0, 1)) * 
			((*this)(1, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(1, 3)) +
			((*this)(0, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(0, 1)) * 
			((*this)(1, 2) * (*this)(2, 3) - (*this)(2, 2) * (*this)(1, 3)) + 
			((*this)(1, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(1, 1)) * 
			((*this)(0, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(0, 3)) -
			((*this)(1, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(1, 1)) * 
			((*this)(0, 2) * (*this)(2, 3) - (*this)(2, 2) * (*this)(0, 3)) + 
			((*this)(2, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(2, 1)) * 
			((*this)(0, 2) * (*this)(1, 3) - (*this)(1, 2) * (*this)(0, 3));

	return det;
}

bool EMATRIX::reverse() {
	float d = determinant();
	if(d == 0.0f) return false; 
	// tutaj cosnie tak mo¿e byæ, mo¿e 
	// nie byæ odwrotnoœci a det bêdzie bardzo maly

	d = 1.0f / d;

	EMATRIX mat;

	mat(0, 0) = d * ((*this)(1, 1) * ((*this)(2, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(2, 3)) + (*this)(2, 1) * ((*this)(3, 2) * (*this)(1, 3) - (*this)(1, 2) * (*this)(3, 3)) + (*this)(3, 1) * ((*this)(1, 2) * (*this)(2, 3) - (*this)(2, 2) * (*this)(1, 3)));
	mat(1, 0) = d * ((*this)(1, 2) * ((*this)(2, 0) * (*this)(3, 3) - (*this)(3, 0) * (*this)(2, 3)) + (*this)(2, 2) * ((*this)(3, 0) * (*this)(1, 3) - (*this)(1, 0) * (*this)(3, 3)) + (*this)(3, 2) * ((*this)(1, 0) * (*this)(2, 3) - (*this)(2, 0) * (*this)(1, 3)));
	mat(2, 0) = d * ((*this)(1, 3) * ((*this)(2, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(2, 1)) + (*this)(2, 3) * ((*this)(3, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(3, 1)) + (*this)(3, 3) * ((*this)(1, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(1, 1)));
	mat(3, 0) = d * ((*this)(1, 0) * ((*this)(3, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(3, 2)) + (*this)(2, 0) * ((*this)(1, 1) * (*this)(3, 2) - (*this)(3, 1) * (*this)(1, 2)) + (*this)(3, 0) * ((*this)(2, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(2, 2)));
	mat(0, 1) = d * ((*this)(2, 1) * ((*this)(0, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(0, 3)) + (*this)(3, 1) * ((*this)(2, 2) * (*this)(0, 3) - (*this)(0, 2) * (*this)(2, 3)) + (*this)(0, 1) * ((*this)(3, 2) * (*this)(2, 3) - (*this)(2, 2) * (*this)(3, 3)));
	mat(1, 1) = d * ((*this)(2, 2) * ((*this)(0, 0) * (*this)(3, 3) - (*this)(3, 0) * (*this)(0, 3)) + (*this)(3, 2) * ((*this)(2, 0) * (*this)(0, 3) - (*this)(0, 0) * (*this)(2, 3)) + (*this)(0, 2) * ((*this)(3, 0) * (*this)(2, 3) - (*this)(2, 0) * (*this)(3, 3)));
	mat(2, 1) = d * ((*this)(2, 3) * ((*this)(0, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(0, 1)) + (*this)(3, 3) * ((*this)(2, 0) * (*this)(0, 1) - (*this)(0, 0) * (*this)(2, 1)) + (*this)(0, 3) * ((*this)(3, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(3, 1)));
	mat(3, 1) = d * ((*this)(2, 0) * ((*this)(3, 1) * (*this)(0, 2) - (*this)(0, 1) * (*this)(3, 2)) + (*this)(3, 0) * ((*this)(0, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(0, 2)) + (*this)(0, 0) * ((*this)(2, 1) * (*this)(3, 2) - (*this)(3, 1) * (*this)(2, 2)));
	mat(0, 2) = d * ((*this)(3, 1) * ((*this)(0, 2) * (*this)(1, 3) - (*this)(1, 2) * (*this)(0, 3)) + (*this)(0, 1) * ((*this)(1, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(1, 3)) + (*this)(1, 1) * ((*this)(3, 2) * (*this)(0, 3) - (*this)(0, 2) * (*this)(3, 3)));
	mat(1, 2) = d * ((*this)(3, 2) * ((*this)(0, 0) * (*this)(1, 3) - (*this)(1, 0) * (*this)(0, 3)) + (*this)(0, 2) * ((*this)(1, 0) * (*this)(3, 3) - (*this)(3, 0) * (*this)(1, 3)) + (*this)(1, 2) * ((*this)(3, 0) * (*this)(0, 3) - (*this)(0, 0) * (*this)(3, 3)));
	mat(2, 2) = d * ((*this)(3, 3) * ((*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1)) + (*this)(0, 3) * ((*this)(1, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(1, 1)) + (*this)(1, 3) * ((*this)(3, 0) * (*this)(0, 1) - (*this)(0, 0) * (*this)(3, 1)));
	mat(3, 2) = d * ((*this)(3, 0) * ((*this)(1, 1) * (*this)(0, 2) - (*this)(0, 1) * (*this)(1, 2)) + (*this)(0, 0) * ((*this)(3, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(3, 2)) + (*this)(1, 0) * ((*this)(0, 1) * (*this)(3, 2) - (*this)(3, 1) * (*this)(0, 2)));
	mat(0, 3) = d * ((*this)(0, 1) * ((*this)(2, 2) * (*this)(1, 3) - (*this)(1, 2) * (*this)(2, 3)) + (*this)(1, 1) * ((*this)(0, 2) * (*this)(2, 3) - (*this)(2, 2) * (*this)(0, 3)) + (*this)(2, 1) * ((*this)(1, 2) * (*this)(0, 3) - (*this)(0, 2) * (*this)(1, 3)));
	mat(1, 3) = d * ((*this)(0, 2) * ((*this)(2, 0) * (*this)(1, 3) - (*this)(1, 0) * (*this)(2, 3)) + (*this)(1, 2) * ((*this)(0, 0) * (*this)(2, 3) - (*this)(2, 0) * (*this)(0, 3)) + (*this)(2, 2) * ((*this)(1, 0) * (*this)(0, 3) - (*this)(0, 0) * (*this)(1, 3)));
	mat(2, 3) = d * ((*this)(0, 3) * ((*this)(2, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(2, 1)) + (*this)(1, 3) * ((*this)(0, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(0, 1)) + (*this)(2, 3) * ((*this)(1, 0) * (*this)(0, 1) - (*this)(0, 0) * (*this)(1, 1)));
	mat(3, 3) = d * ((*this)(0, 0) * ((*this)(1, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(1, 2)) + (*this)(1, 0) * ((*this)(2, 1) * (*this)(0, 2) - (*this)(0, 1) * (*this)(2, 2)) + (*this)(2, 0) * ((*this)(0, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(0, 2)));

	(*this) = mat;

	return true;
}

void EMATRIX::translate(float _x, float _y, float _z) {
	m[3][0] = _x;
	m[3][1] = _y;
	m[3][2] = _z;
}

void EMATRIX::scale(float _x, float _y, float _z) {
	m[0][0] = _x;
	m[1][1] = _y;
	m[2][2] = _z;
}

void EMATRIX::perspectiveFovLH(float angel, float aspect, float zNear, float zFar) {
	/*	xScale     0          0               0
		0        yScale       0               0
		0          0       zf/(zf-zn)         1
		0          0       -zn*zf/(zf-zn)     0
			where:
			yScale = cot(fovY/2)

			xScale = yScale / aspect ratio		*/

	identity();

	float yScale = 1 / tan(angel/2);
	float xScale = yScale / aspect;

	m[0][0] = xScale;
	m[1][1] = yScale;
	m[2][2] = zFar/(zFar-zNear);			m[2][3] = 1;
	m[3][2] = -zNear*zFar/(zFar-zNear);
}

void EMATRIX::lookAtLH(EVECTOR3 pos, EVECTOR3 lookAt, EVECTOR3 up) {
	/*	zaxis = normal(At - Eye)
		xaxis = normal(cross(Up, zaxis))
		yaxis = cross(zaxis, xaxis)
    
		xaxis.x           yaxis.x           zaxis.x          0
		xaxis.y           yaxis.y           zaxis.y          0
		xaxis.z           yaxis.z           zaxis.z          0
		-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1 */

	identity();
	
	EVECTOR3 zAxis = lookAt-pos;
	zAxis.normalize();
	EVECTOR3 xAxis = up.crossProduct(zAxis);
	xAxis.normalize();
	EVECTOR3 yAxis = zAxis.crossProduct(xAxis);

	m[0][0] = xAxis[0];	m[0][1] = yAxis[0];	m[0][2] = zAxis[0];
	m[1][0] = xAxis[1];	m[1][1] = yAxis[1];	m[1][2] = zAxis[1];
	m[2][0] = xAxis[2];	m[2][1] = yAxis[2];	m[2][2] = zAxis[2];
	m[3][0] = -xAxis.dotProduct(pos); 
						m[3][1] = -yAxis.dotProduct(pos);
											m[3][2] = -zAxis.dotProduct(pos);
}

void EMATRIX::rotateX(float _ang) {
	identity();

	m[1][1] = cos(_ang); m[1][2] = sin(_ang);
	m[2][1] = -sin(_ang); m[2][2] = cos(_ang);
}

void EMATRIX::rotateY(float _ang) {
	identity();

	m[0][0] = cos(_ang); m[0][2] = -sin(_ang);
	m[2][0] = sin(_ang); m[2][2] = cos(_ang);
}

void EMATRIX::rotateZ(float _ang) {
	identity();

	m[0][0] = cos(_ang); m[0][1] = sin(_ang);
	m[1][0] = -sin(_ang); m[1][1] = cos(_ang);
}

void EMATRIX::rotateAxis(float _ang, const EVECTOR3& _ax) {
	float cosa = cos(_ang);
	float sina = sin(_ang);
	float omc = 1 - cosa;

	identity();

	m[0][0] = _ax[0]*_ax[0] + (1 - _ax[0]*_ax[0]) * cosa;	m[0][1] = _ax[0]*_ax[1]*omc - _ax[2]*sina;				m[0][2] = _ax[0]*_ax[2]*omc + _ax[1]*sina;
	m[1][0] = _ax[0]*_ax[1]*omc + _ax[2]*sina;				m[1][1] = _ax[1]*_ax[1] + (1 - _ax[1]*_ax[1]) * cosa;	m[1][2] = _ax[1]*_ax[2]*omc - _ax[0]*sina;
	m[2][0] = _ax[0]*_ax[2]*omc - _ax[1]*sina;				m[2][1] = _ax[1]*_ax[2]*omc + _ax[0]*sina;				m[2][2] = _ax[2]*_ax[2] + (1 - _ax[2]*_ax[2]) * cosa;
}

float& EMATRIX::operator[](int _i) {
	return m[_i/4][_i%4];
}

float EMATRIX::operator[](int _i) const {
	return m[_i/4][_i%4];
}

float EMATRIX::operator()(int i, int j) const {
	return m[i][j];
}

float& EMATRIX::operator()(int i, int j) {
	return m[i][j];
}

const EMATRIX EMATRIX::operator+(const EMATRIX& _r) {
	EMATRIX ret;

	for(unsigned char i = 0; i < 16; ++i) {
		ret[i] = this->operator[](i) + _r[i];
	}

	return ret;
}

const EMATRIX EMATRIX::operator-(const EMATRIX& _r) {
	EMATRIX ret;

	for(unsigned char i = 0; i < 16; ++i) {
		ret[i] = this->operator[](i) - _r[i];
	}

	return ret;
}

const EMATRIX EMATRIX::operator*(const float _r) {
	EMATRIX ret;
	
	for(unsigned char i = 0; i < 16; ++i) {
		ret[i] = this->operator[](i) * _r;
	}

	return ret;
}

const EMATRIX EMATRIX::operator*(const EMATRIX& _r) {
	EMATRIX ret;

	for(unsigned char k = 0; k < 4; ++k) {
		for(unsigned char j = 0; j < 4; ++j) {
			for(unsigned char i = 0; i < 4; ++i) {
				ret(k,j) += _r(k,i) * (*this)(i,j);
			}
		}
	}

	return ret;
}

const EVECTOR EMATRIX::operator*(const EVECTOR& _r) {
	EVECTOR ret;

	ret[0] = _r[0] * m[0][0] + _r[1] * m[1][0] + _r[2] * m[2][0] + _r[3] * m[3][0];
	ret[1] = _r[0] * m[0][1] + _r[1] * m[1][1] + _r[2] * m[2][1] + _r[3] * m[3][1];
	ret[2] = _r[0] * m[0][2] + _r[1] * m[1][2] + _r[2] * m[2][2] + _r[3] * m[3][2];
	ret[3] = _r[0] * m[0][3] + _r[1] * m[1][3] + _r[2] * m[2][3] + _r[3] * m[3][3];

	return ret;
}

const EVECTOR3 EMATRIX::operator*(const EVECTOR3& _ra) {
	EVECTOR _r(_ra[0], _ra[1], _ra[2]);
	EVECTOR ret;

	ret[0] = _r[0] * m[0][0] + _r[1] * m[1][0] + _r[2] * m[2][0] + _r[3] * m[3][0];
	ret[1] = _r[0] * m[0][1] + _r[1] * m[1][1] + _r[2] * m[2][1] + _r[3] * m[3][1];
	ret[2] = _r[0] * m[0][2] + _r[1] * m[1][2] + _r[2] * m[2][2] + _r[3] * m[3][2];
	ret[3] = _r[0] * m[0][3] + _r[1] * m[1][3] + _r[2] * m[2][3] + _r[3] * m[3][3];

	// TUTAJ COS MOZE BYC NIESCISLEGO!!

	EVECTOR3 a(ret[0], ret[1], ret[2]);
	return a;
}

bool EMATRIX::operator==(const EMATRIX& right) const {
	for(unsigned int i = 0; i < 16; ++i) 
		if(mat[i] != right.mat[i]) return false;
	return true;
}

EPLANE::EPLANE() : a(0.0f), b(0.0f), c(0.0f), d(0.0f) {
}

EPLANE::EPLANE(float _a, float _b, float _c, float _d) 
	: a(_a), b(_b), c(_c), d(_d) {

}

EPLANE::EPLANE(const EVECTOR3& p1, const EVECTOR3& p2, const EVECTOR3& p3) {
	make(p1,p2,p3);
}

void EPLANE::make(const EVECTOR3& p1, const EVECTOR3& p2, const EVECTOR3& p3) {
	// nx(x - x0) + ny(y - y0) + nz(z - z0) = 0
	EVECTOR3 p = p2-p1;
	EVECTOR3 n = p.crossProduct(p3-p1);
	n.normalize();
	
	make(n, p1);
}

void EPLANE::make(const EVECTOR3& norm, const EVECTOR3& pos) {
	a = norm.x;
	b = norm.y;
	c = norm.z;
	d = - pos.x * a - pos.y * b - pos.z * c;
}

EFRUSTUM::EFRUSTUM() { 
}

EFRUSTUM::EFRUSTUM(float fov, float a, float znear, float zfar, 
	const EVECTOR3& pos, const EVECTOR3& dir, const EVECTOR3& up) {
		make(fov, a, znear, zfar, pos, dir, up);
}

void EFRUSTUM::make(float fov, float aratio, float znear, float zfar, 
	const EVECTOR3& pos, const EVECTOR3& dir, const EVECTOR3& up) {
		EVECTOR3 Zaxis, Xaxis, Yaxis, ncenter, fcenter;
		float fovTan = 0.0f, nearH = 0.0f, nearW = 0.0f, farH = 0.0f, farW = 0.0f;

		fovTan = tanf(fov);
		
		Zaxis = dir;
		Zaxis.normalize();
		Xaxis = up.crossProduct(Zaxis);
		Xaxis.normalize();
		Yaxis = Zaxis.crossProduct(Xaxis);
		Yaxis.normalize();

		ncenter = pos + (Zaxis*znear);
		fcenter = pos + (Zaxis*zfar);

		nearW = znear * fovTan;
		nearH = nearW / aratio; 
		farW = zfar  * fovTan;
		farH = farW / aratio;

		plane[0].make(dir, ncenter);
		plane[1].make(-dir, fcenter);

		plane[2].make(fcenter-(Xaxis*farW), ncenter-(Xaxis*nearW), ncenter-(Xaxis*nearW)+(Yaxis*nearH));
		plane[3].make(ncenter+(Xaxis*nearW), fcenter+(Xaxis*farW), ncenter+(Xaxis*nearW)+(Yaxis*nearH));

		plane[4].make(ncenter+(Yaxis*nearH), fcenter+(Yaxis*farH), ncenter-(Xaxis*nearW)+(Yaxis*nearH));
		plane[5].make(fcenter-(Yaxis*farH), ncenter-(Yaxis*nearH), ncenter-(Xaxis*nearW)-(Yaxis*nearH));
}

std::ostream& operator<<(std::ostream& _l, const EVECTOR& _r) {
	for(unsigned char i = 0; i < 4; ++i) {
		_l << _r[i] << " ";
	}

	return _l;
}

std::ostream& operator<<(std::ostream& _l, const EVECTOR3& _r) {
	for(unsigned char i = 0; i < 3; ++i) {
		_l << _r[i] << " ";
	}

	return _l;
}

std::ostream& operator<<(std::ostream& _l, const EMATRIX& _r) {
	for(unsigned char i = 0; i < 4; ++i) {
		for(unsigned char j = 0; j < 4; ++j) {
			_l << _r(i, j) << " ";
		}
		_l << "\n";
	}

	return _l;
}

std::ostream& operator<<(std::ostream& _l, const EPLANE& _r) {
	_l << _r.a << "x + " << _r.b << "y + " << _r.c << "z + " << _r.d << " = 0\n";

	return _l;
}
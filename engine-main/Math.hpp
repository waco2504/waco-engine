#pragma once

#ifndef _INC_MATH_
#define _INC_MATH_

#include <iostream>

class EVECTOR3;

class EVECTOR {
public:
	float x;
	float y;
	float z;
	float w;

	EVECTOR(const EVECTOR3&);
	EVECTOR(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, 
			float _w = 1.0f);

	float& operator[](int index);
	float operator[](int index) const;
	const EVECTOR operator+(const EVECTOR& right) const;
	const EVECTOR operator-(const EVECTOR& right) const;
	bool operator==(const EVECTOR& right) const;
	void operator/=(const float right);
	void operator+=(const EVECTOR& right);

	void normalize();
	float length() const;

	operator EVECTOR3();
};

typedef EVECTOR EVECTOR4;

class EVECTOR3 {
public:
	float x;
	float y;
	float z;

	EVECTOR3(float* v);
	EVECTOR3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f);

	float& operator[](int index);
	float operator[](int index) const;

	void normalize();

	const float length() const;
	const float dotProduct(const EVECTOR3& right) const;
	const EVECTOR3 crossProduct(const EVECTOR3& right) const;

	const EVECTOR3 operator+(const EVECTOR3& right) const;
	const EVECTOR3 operator-(const EVECTOR3& right) const;

	const EVECTOR3 operator*(const EVECTOR3& right) const;
	const EVECTOR3 operator*(const float right) const;

	const EVECTOR3 operator-() const;

	void operator+=(const EVECTOR3& right);
	void operator*=(const float right);
	void operator/=(const float right);

	bool operator==(const EVECTOR3& r) const;
};

class EMATRIX {
public:
	union {
		float m[4][4];
		float mat[16];
	};

	EMATRIX();
	EMATRIX(float* mat);
	EMATRIX(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13, 
			float m20, float m21, float m22, float m23, 
			float m30, float m31, float m32, float m33 );

	void identity();
	void transpose();
	float determinant();
	bool reverse();
	void translate(float x, float y, float z);
	void scale(float x, float y, float z);
	// do tego funkcje tworz¹c urzytkowe macierze, od prostackich skalowan,
	void perspectiveFovLH(float angle, float aspect, float zNear, float zFar);
	void lookAtLH(EVECTOR3 pos, EVECTOR3 lookAt, EVECTOR3 up);
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
	void rotateAxis(float angle, const EVECTOR3& axis);
	// obrotów po projekcji lookat i innych ortho2d tez

	float operator[](int idx) const;
	float& operator[](int idx);
	float operator()(int i, int j) const;
	float& operator()(int i, int j);

	const EMATRIX operator+(const EMATRIX& right);
	const EMATRIX operator-(const EMATRIX& right);

	const EMATRIX operator*(const float right);
	const EMATRIX operator*(const EMATRIX& right);
	const EVECTOR operator*(const EVECTOR& right);
	const EVECTOR3 operator*(const EVECTOR3& right);

	bool operator==(const EMATRIX& right) const;
};

class EPLANE {
public:
	float a;
	float b;
	float c;
	float d;
	
	EPLANE();
	EPLANE(float _a, float _b, float _c, float _d);
	EPLANE(const EVECTOR3& p1, const EVECTOR3& p2, const EVECTOR3& p3);
	void make(const EVECTOR3& p1, const EVECTOR3& p2, const EVECTOR3& p3);
	void make(const EVECTOR3& normal, const EVECTOR3& point);
};

class EFRUSTUM {
public:
	EPLANE plane[6]; // tworzone w postaci 'normalnej'

	EFRUSTUM();
	EFRUSTUM(float fov, float aspect, float znear, float zfar, 
		const EVECTOR3& position, const EVECTOR3& direction, const EVECTOR3& up); 
	void make(float fov, float aspect, float znear, float zfar, 
		const EVECTOR3& position, const EVECTOR3& direction, const EVECTOR3& up);
};

std::ostream& operator<<(std::ostream& left, const EVECTOR& right);
std::ostream& operator<<(std::ostream& left, const EVECTOR3& right);
std::ostream& operator<<(std::ostream& left, const EMATRIX& right);
std::ostream& operator<<(std::ostream& left, const EPLANE& right);

#endif
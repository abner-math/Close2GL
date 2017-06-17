#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include <iostream>

class Vector3f
{
public:
	float x, y, z, w;

	Vector3f(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
	Vector3f operator+() const;
	Vector3f operator-() const;
	Vector3f operator+(const Vector3f &v);	
	Vector3f operator-(const Vector3f &v);
	Vector3f& operator=(const Vector3f &v);
	Vector3f& operator+=(const Vector3f &v);
	Vector3f& operator-=(const Vector3f &v);
	Vector3f operator*(const float scalar);
	Vector3f operator/(const float scalar);
	float operator&(const Vector3f &v);
	Vector3f operator^(const Vector3f &v);
	float length() const;
	Vector3f normalized();
	friend std::ostream& operator<<(std::ostream &os, const Vector3f &other);
};

#endif // _VECTOR3F_H_

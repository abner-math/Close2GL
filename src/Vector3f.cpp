#include "Vector3f.h"

#include <cmath>

Vector3f::Vector3f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{

}

Vector3f Vector3f::operator+() const
{
	return Vector3f(x, y, z, w);
}

Vector3f Vector3f::operator-() const
{
	return Vector3f(-x, -y, -z, -w);
}

Vector3f Vector3f::operator+(const Vector3f &v) 
{
	Vector3f res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	res.w = w + v.w;
	return res;
}

Vector3f Vector3f::operator-(const Vector3f &v) 
{
	Vector3f res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	res.w = z - v.w;
	return res;
}

Vector3f& Vector3f::operator=(const Vector3f &v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

Vector3f& Vector3f::operator+=(const Vector3f &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

Vector3f Vector3f::operator*(const float scalar) 
{
	Vector3f res;
	res.x = x * scalar;
	res.y = y * scalar;
	res.z = z * scalar;
	res.w = w * scalar;
	return res;
}

Vector3f Vector3f::operator/(const float scalar) 
{
	Vector3f res;
	res.x = x / scalar;
	res.y = y / scalar;
	res.z = z / scalar;
	res.w = w / scalar;
	return res;
}

float Vector3f::operator&(const Vector3f &v)
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3f Vector3f::operator^(const Vector3f &v)
{
	Vector3f res;
    res.x =  y * v.z - z * v.y;
    res.y = -x * v.z + z * v.x;
    res.z =  x * v.y - y * v.x;
    return res;
}

std::ostream& operator<<(std::ostream &os, const Vector3f &other)
{
	os << "[" << other.x << "," << other.y << "," << other.z << "," << other.w << "]";
	return os;
}

float Vector3f::length() const
{
	return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
}

Vector3f Vector3f::normalized()
{
	Vector3f res;
	float len = length();
	res.x = x / len;
	res.y = y / len;
	res.z = z / len;
	return res; 
}

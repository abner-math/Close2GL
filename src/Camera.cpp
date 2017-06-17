#include "Camera.h"

Camera::Camera(Vector3f origin, Vector3f target) 
 : startingOrigin(origin), target(target)
{
	reset();
}

void Camera::reset()
{
	origin = startingOrigin;
	U = Vector3f(1.0f, 0.0f, 0.0f);
	V = Vector3f(0.0f, 1.0f, 0.0f);
	N = Vector3f(0.0f, 0.0f, -1.0f);
	translation = Vector3f(0.0f, 0.0f, 0.0f);
	rotation = Vector3f(0.0f, 0.0f, 0.0f);
}

void Camera::lookAtTarget()
{
	origin = origin - U * translation.x - V * translation.y - N * translation.z;
	translation = Vector3f(0.0f, 0.0f, 0.0f);
	rotation = Vector3f(0.0f, 0.0f, 0.0f);
	N = (origin - target).normalized();
	U = (N ^ V).normalized();
	V = (U ^ N).normalized();
}

void Camera::roll(Vector3f &u, Vector3f &v, Vector3f &n)
{
	float alpha = rotation.y * M_PI / 180;
	Vector3f u_ = u * std::cos(alpha) + n * std::sin(alpha);
	Vector3f n_ = -u * std::sin(alpha) + n * std::cos(alpha);
	u = u_;
	n = n_;
}

void Camera::pitch(Vector3f &u, Vector3f &v, Vector3f &n)
{
	float alpha = rotation.x * M_PI / 180;
	Vector3f v_ = v * std::cos(alpha) + n * std::sin(alpha);
	Vector3f n_ = -v * std::sin(alpha) + n * std::cos(alpha);
	v = v_;
	n = n_;
}

void Camera::yaw(Vector3f &u, Vector3f &v, Vector3f &n)
{
	float alpha = rotation.z * M_PI / 180;
	Vector3f u_ = u * std::cos(alpha) + v * std::sin(alpha);
	Vector3f v_ = -u * std::sin(alpha) + v * std::cos(alpha);
	u = u_;
	v = v_;
}

void Camera::loadModelViewMatrix()
{
	Vector3f u = U;
	Vector3f v = V;
	Vector3f n = N;
	roll(u, v, n);
	pitch(u, v, n);
	yaw(u, v, n);
	Vector3f translatedOrigin = origin - u * translation.x - v * translation.y - n * translation.z;
	float dotU = translatedOrigin & u;
	float dotV = translatedOrigin & v;
	float dotN = translatedOrigin & n;
	float viewMatrix[16];
	viewMatrix[0] = u.x;	viewMatrix[1] = u.y;	viewMatrix[2] = u.z;	viewMatrix[3] = -dotU;
	viewMatrix[4] = v.x;	viewMatrix[5] = v.y;	viewMatrix[6] = v.z;	viewMatrix[7] = -dotV;
	viewMatrix[8] = n.x;	viewMatrix[9] = n.y;	viewMatrix[10] = n.z;	viewMatrix[11] = -dotN;
	viewMatrix[12] = 0.0f;	viewMatrix[13] = 0.0f;	viewMatrix[14] = 0.0f;	viewMatrix[15] = 1.0f;
	glMultTransposeMatrixf(viewMatrix);
}

Matrix Camera::loadCloseModelViewMatrix()
{
	Vector3f u = U;
	Vector3f v = V;
	Vector3f n = N;
	roll(u, v, n);
	pitch(u, v, n);
	yaw(u, v, n);
	Vector3f translatedOrigin = origin - u * translation.x - v * translation.y - n * translation.z;
	float dotU = translatedOrigin & u;
	float dotV = translatedOrigin & v;
	float dotN = translatedOrigin & n;
	Matrix modelView(4, 4);
	modelView(0, 0) = u.x;	modelView(0, 1) = u.y;	modelView(0, 2) = u.z;	modelView(0, 3) = -dotU;
	modelView(1, 0) = v.x;	modelView(1, 1) = v.y;	modelView(1, 2) = v.z;	modelView(1, 3) = -dotV;
	modelView(2, 0) = n.x;	modelView(2, 1) = n.y;	modelView(2, 2) = n.z;	modelView(2, 3) = -dotN;
	modelView(3, 0) = 0.0f;	modelView(3, 1) = 0.0f; modelView(3, 2) = 0.0f;	modelView(3, 3) = 1.0f;
	return modelView;
}

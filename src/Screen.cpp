#include "Screen.h"

Screen::Screen()
{
	reset();
}

void Screen::reset()
{
	fovH = fovV = 60.0f;
	zNear = 1.0f;
	zFar = 3000.0f;
}

void Screen::loadViewportMatrix(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void Screen::loadProjectionMatrix()
{
	gluPerspective(fovV, fovH/fovV, zNear, zFar);
}

Matrix Screen::loadCloseViewportMatrix(int w, int h)
{
	Matrix viewport(4, 4);
	viewport(0, 0) = w/2;		viewport(0, 1) = 0;		viewport(0, 2) = 0;		viewport(0, 3) = w/2;
	viewport(1, 0) = 0;			viewport(1, 1) = h/2;	viewport(1, 2) = 0;		viewport(1, 3) = h/2;
	viewport(2, 0) = 0;			viewport(2, 1) = 0;		viewport(2, 2) = 1;		viewport(2, 3) = 0;
	viewport(3, 0) = 0;			viewport(3, 1) = 0;		viewport(3, 2) = 0;		viewport(3, 3) = 1;
	return viewport;
}

Matrix Screen::loadCloseProjectionMatrix()
{
	float ymax = zNear * std::tan(fovV * M_PI / 360);
	float xmax = ymax * fovH / fovV;
	float n = zNear;
	float f = zFar;
	float l = -xmax;
	float r = xmax;
	float b = -ymax;
	float t = ymax;
	Matrix projection(4, 4);
	projection(0, 0) = 2*n/(r-l);	projection(0, 1) = 0;			projection(0, 2) = (r+l)/(r-l);		projection(0, 3) = 0;
	projection(1, 0) = 0;			projection(1, 1) = 2*n/(t-b);	projection(1, 2) = (t+b)/(t-b);		projection(1, 3) = 0;
	projection(2, 0) = 0;			projection(2, 1) = 0;			projection(2, 2) = -(f+n)/(f-n);	projection(2, 3) = -2*f*n/(f-n);
	projection(3, 0) = 0;			projection(3, 1) = 0;			projection(3, 2) = -1;				projection(3, 3) = 0;
	return projection;
}

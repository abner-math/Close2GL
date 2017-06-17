#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <cmath>
#include <GL/glut.h>
#include <GL/glu.h>
#include "Vector3f.h"
#include "Matrix.h"

class Screen
{
public:
	Screen();
	
	void reset();
	
	void loadViewportMatrix(int w, int h);
	
	void loadProjectionMatrix();
	
	Matrix loadCloseViewportMatrix(int w, int h);
	
	Matrix loadCloseProjectionMatrix();
	
	float& getFovH()
	{
		return fovH;
	}
	
	float& getFovV()
	{
		return fovV;
	}
	
	float& getZNear()
	{
		return zNear;
	}
	
	float& getZFar()
	{
		return zFar;
	}
	
private:
	float fovH, fovV, zNear, zFar;
	
};

#endif // _SCREEN_H_

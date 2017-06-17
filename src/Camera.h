#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <cmath>
#include <GL/glut.h>
#include <GL/glu.h>
#include "Vector3f.h"
#include "Matrix.h"

class Camera
{
public:
	Vector3f U, V, N, origin, startingOrigin, target, translation, rotation;

	Camera(Vector3f origin = Vector3f(), Vector3f target = Vector3f());

	void reset();
	
	void lookAtTarget();
	
	void loadModelViewMatrix();
	
	Matrix loadCloseModelViewMatrix();
	
	Vector3f& getTranslation() 
	{
		return translation;
	}
	
	Vector3f& getRotation() 
	{
		return rotation;
	}
	
private:
	void roll(Vector3f &u, Vector3f &v, Vector3f &n);
	
	void pitch(Vector3f &u, Vector3f &v, Vector3f &n);

	void yaw(Vector3f &u, Vector3f &v, Vector3f &n);

};

#endif // _CAMERA_H_

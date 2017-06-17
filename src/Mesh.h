#ifndef _MESH_H_
#define _MESH_H_

#include <float.h>
#include "Triangle.h"

class Mesh
{
public:
	Triangle **triangles;
	int numTriangles;
	Vector3f *ambient;
	Vector3f *diffuse;
	Vector3f *specular;
	float *shine;
	int materialCount;
	bool texture;
	
	Mesh(Triangle **triangles, int numTriangles, Vector3f *ambient, Vector3f *diffuse, 
		Vector3f *specular, float *shine, int materialCount, bool texture);
	~Mesh();
	
	void getMinMax(Vector3f &min, Vector3f &max);
	
};

#endif // _MESH_H_

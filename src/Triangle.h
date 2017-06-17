#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "Vector3f.h"

class Triangle
{
public:
	Vector3f vertices[3], projectedVertices[3], colors[3], normals[3], textureCoords[3], faceNormal;
	int colorIndex[3];	
};

#endif // _TRIANGLE_H_

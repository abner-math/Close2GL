#include "Mesh.h"

Mesh::Mesh(Triangle **triangles, int numTriangles, Vector3f *ambient, 
Vector3f *diffuse, Vector3f *specular, float *shine, int materialCount, bool texture) 
: triangles(triangles), numTriangles(numTriangles), ambient(ambient), diffuse(diffuse), 
specular(specular), shine(shine), materialCount(materialCount), texture(texture)
{

}

Mesh::~Mesh()
{
	for (int i = 0; i < numTriangles; i++)
	{
		delete triangles[i];
	}
	delete[] triangles;
	delete[] ambient;
	delete[] diffuse;
	delete[] specular;
	delete[] shine;
}

void Mesh::getMinMax(Vector3f &meshMin, Vector3f &meshMax)
{
	meshMin = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	meshMax = Vector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int triangleIdx = 0; triangleIdx < numTriangles; triangleIdx++)
	{
		for (int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
		{
			Vector3f vertex = triangles[triangleIdx]->vertices[vertexIdx];
			if (vertex.x > meshMax.x) meshMax.x = vertex.x;
			if (vertex.y > meshMax.y) meshMax.y = vertex.y;
			if (vertex.z > meshMax.z) meshMax.z = vertex.z;
			if (vertex.x < meshMin.x) meshMin.x = vertex.x;
			if (vertex.y < meshMin.y) meshMin.y = vertex.y;
			if (vertex.z < meshMin.z) meshMin.z = vertex.z;
		}
	}	
}

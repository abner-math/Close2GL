#ifndef _RENDERER_H_
#define _RENDERER_H_

#define RENDER_MODE_POINT 0
#define RENDER_MODE_WIREFRAME 1
#define RENDER_MODE_SOLID 2
#define FRONT_FACE_CLOCKWISE 0
#define FRONT_FACE_COUNTERCLOCKWISE 1
#define LIGHTING_ON 1
#define LIGHTING_OFF 0
#define SHADING_MODE_FLAT 0
#define SHADING_MODE_GOURAUD 1
#define SHADING_MODE_NONE 2
#define TEXTURE_FILTERING_NEAREST_NEIGHBORS 0
#define TEXTURE_FILTERING_BILINEAR 1
#define TEXTURE_FILTERING_MIPMAP 2

#include <GL/glut.h>
#include <GL/glu.h>
#include <vector>
#include "Vector3f.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Camera.h"
#include "Screen.h"
#include "Buffer.h"

class Renderer
{
public:
	Renderer();

	void reset();
	
	int& getRenderMode()
	{
		return renderMode;
	}
	
	int& getShadingMode()
	{
		return shadingMode;
	}
	
	int& getFrontFace()
	{
		return frontFace;
	}
	
	Vector3f& getObjectColor()
	{
		return objectColor;
	}
	
	Vector3f& getAmbientColor()
	{
		return ambientColor;
	}
	
	Vector3f& getSpecularColor()
	{
		return specularColor;
	}
	
	Vector3f& getLightColor()
	{
		return lightColor;
	}

	Vector3f& getLightPosition()
	{
		return lightPosition;
	}
		
	float& getShininess()
	{
		return shininess;
	}

	float& getAttenuationConstant()
	{
		return attenuationConstant;
	}
	
	float& getAttenuationLinear()
	{
		return attenuationLinear;
	}
	
	float& getAttenuationQuadratic()
	{
		return attenuationQuadratic;
	}
	
	void setTexture(float *texture, int textName, int textW, int textH)
	{
		this->texture = texture;
		this->textName = textName;
		this->textW = textW;
		this->textH = textH;
		createMipmaps();
	}
	
	float* getTexture()
	{
		return texture;
	}
	
	int& getTextureFiltering()
	{
		return textureFiltering;
	}
	
	int& getPerspectiveCorrect()
	{
		return perspectiveCorrect;
	}
	
	void removeTexture()
	{
		delete[] texture;
		for (int i = 0; i < mipmapLevels; i++)
		{
			delete[] textureMipmaps[i];
		}
		delete[] textureMipmaps;
	}
	
	void renderGL(Mesh *mesh);
	
	void renderClose2GL(Mesh *mesh, Camera &camera, Screen &screen, int w, int h);
	
private:
	Vector3f objectColor, ambientColor, specularColor, lightColor, lightPosition;
	int renderMode, shadingMode, frontFace, textureFiltering, textName, textW, textH, perspectiveCorrect, mipmapLevels;
	float shininess, attenuationConstant, attenuationLinear, attenuationQuadratic, *texture, **textureMipmaps;
	
	void setupRendererGL();
	
	Matrix getVertices(Mesh *mesh);
	
	Matrix perspectiveDivision(Matrix &projected);
	
	std::vector<int> clip(Matrix &vertices, int width, int height);
	
	std::vector<int> backfaceCulling(Mesh *mesh, Vector3f &cameraPosition, std::vector<int> &triangles);
	
	Vector3f calculatePhongIlluminationModel(Vector3f &cameraPosition, Vector3f vertexPosition, Vector3f vertexNormal);
	
	void sortVerticesByY(Triangle *triangle, int &minVertex1, int &minVertex2, int &minVertex3);

	void drawPixel(Buffer *buffer, int x, int y, float z, Vector3f color);
	
	void drawLine(Buffer *buffer, int x1, int x2, int y, float z1, float z2, float w1, float w2, Vector3f c1, Vector3f c2, Vector3f t1, Vector3f t2, float mipmapLevel, bool hasTexture, bool wireframe);
	
	void rasterizeTriangle(Triangle *triangle, Buffer *buffer, bool hasTexture);
	
	void rasterizeVertices(Triangle *triangle, Buffer *buffer, bool hasTexture);
	
	Vector3f getTextureColor(Vector3f coords);
	
	Vector3f getTextureColorMipmap(Vector3f coords, int level);
	
	Vector3f sampleTexture(Vector3f coords, float mipmapLevel);
	
	float* downsample(float *texture, int w, int h);
	
	void createMipmaps();
	
};

#endif // _RENDERER_H_

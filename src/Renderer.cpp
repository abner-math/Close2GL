#include "Renderer.h"
#include <iostream>
#include <algorithm>
#include <string.h>

Renderer::Renderer()
{
	reset();
	texture = nullptr;
}

void Renderer::reset()
{
	renderMode = RENDER_MODE_SOLID;
	frontFace = FRONT_FACE_COUNTERCLOCKWISE;
	objectColor = Vector3f(1, 1, 1);
	ambientColor = Vector3f(0, 0, 0);
	specularColor = Vector3f(1, 1, 1);
	lightColor = Vector3f(1, 1, 1);
	lightPosition = Vector3f(200, 100, -700);
	shadingMode = SHADING_MODE_FLAT;
	shininess = 0;
	attenuationConstant = 1;
	attenuationLinear = 0;
	attenuationQuadratic = 0;
	textureFiltering = TEXTURE_FILTERING_NEAREST_NEIGHBORS;
	perspectiveCorrect = 1;
}

void Renderer::setupRendererGL()
{
	switch (renderMode)
	{
	case RENDER_MODE_POINT:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case RENDER_MODE_WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case RENDER_MODE_SOLID:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
	if (frontFace)
	{
		glFrontFace(GL_CCW);
	}
	else
	{
		glFrontFace(GL_CW);
	}
	
	if (shadingMode == SHADING_MODE_FLAT || shadingMode == SHADING_MODE_GOURAUD)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0); 
		float ambientCol[4] = { ambientColor.x, ambientColor.y, ambientColor.z, 1.0f };
		float diffuseCol[4] = { objectColor.x, objectColor.y, objectColor.z, 1.0f };
		float specularCol[4] = { specularColor.x, specularColor.y, specularColor.z, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientCol);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseCol);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularCol);
		float emission[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
		float shine[1] = { (1 - shininess) * 128 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
		float lightCol[3] = { lightColor.x, lightColor.y, lightColor.z };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightCol);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightCol);
		float lightColWhite[3] = { 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightColWhite);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, attenuationConstant);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, attenuationLinear);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, attenuationQuadratic);
		float lightPos[4] = { lightPosition.x, lightPosition.y, lightPosition.z, 1.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
		//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		//float blackCol[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		//glLightModelf(GL_LIGHT_MODEL_LOCAL_AMBIENT, blackCol);
	}
	
	if (shadingMode == SHADING_MODE_FLAT)
	{
		glShadeModel(GL_FLAT);
	}
	else if (shadingMode == SHADING_MODE_GOURAUD)
	{
		glShadeModel(GL_SMOOTH);
	}
	else
	{
		//glDisable(GL_LIGHTING);
		glColor3f(objectColor.x, objectColor.y, objectColor.z);
	}
}
	
void Renderer::renderGL(Mesh *mesh)
{
	if (mesh == nullptr) return;
	setupRendererGL();
	if (mesh->texture)
	{
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, textName);
	}
	glBegin(GL_TRIANGLES);
	for (int triangleIdx = 0; triangleIdx < mesh->numTriangles; triangleIdx++)
	{
		Triangle *triangle = mesh->triangles[triangleIdx];
		for (int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
		{
			if (mesh->texture)
			{
				Vector3f texture = triangle->textureCoords[vertexIdx];
				glTexCoord2f(texture.x, texture.y);
			}
			Vector3f normal = triangle->normals[vertexIdx];
			glNormal3f(normal.x, normal.y, normal.z);	
			Vector3f vertex = triangle->vertices[vertexIdx];
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
	}
	glEnd();
	if (mesh->texture)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

Matrix Renderer::getVertices(Mesh *mesh)
{
	Matrix vertices(4, mesh->numTriangles * 3);
	for (int triangleIdx = 0; triangleIdx < mesh->numTriangles; triangleIdx++)
	{
		for (int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
		{
			Vector3f vertex = mesh->triangles[triangleIdx]->vertices[vertexIdx];
			vertices(0, triangleIdx * 3 + vertexIdx) = vertex.x;
			vertices(1, triangleIdx * 3 + vertexIdx) = vertex.y;
			vertices(2, triangleIdx * 3 + vertexIdx) = vertex.z;
			vertices(3, triangleIdx * 3 + vertexIdx) = 1;
		}
	}
	return vertices;
}

Matrix Renderer::perspectiveDivision(Matrix &projected)
{
	Matrix divided(projected.rows, projected.cols);
	for (int vertexIdx = 0; vertexIdx < projected.cols; vertexIdx++)
	{
		float w = projected(3, vertexIdx);
		if (std::abs(w) < 1e-3)
		{
			divided(0, vertexIdx) = FLT_MAX;
			divided(1, vertexIdx) = FLT_MAX;
			divided(2, vertexIdx) = FLT_MAX;
		}
		else
		{
			divided(0, vertexIdx) = projected(0, vertexIdx) / w;
			divided(1, vertexIdx) = projected(1, vertexIdx) / w;
			divided(2, vertexIdx) = projected(2, vertexIdx) / w;
		}
		divided(3, vertexIdx) = w;
	}
	return divided;
}

std::vector<int> Renderer::clip(Matrix &vertices, int width, int height)
{
	std::vector<int> visibleTriangles;
	for (int triangleIdx = 0; triangleIdx < vertices.cols / 3; triangleIdx++)
	{
		bool isClipped = false;
		for (int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
		{
			float x = vertices(0, triangleIdx * 3 + vertexIdx);
			float y = vertices(1, triangleIdx * 3 + vertexIdx);
			float z = vertices(2, triangleIdx * 3 + vertexIdx);
			if (!(x >= 0 && x <= width && y >= 0 && y <= height && std::abs(z) <= 1))
			{
				isClipped = true;
			}
		}
		if (!isClipped) visibleTriangles.push_back(triangleIdx);
	}
	return visibleTriangles;
}

std::vector<int> Renderer::backfaceCulling(Mesh *mesh, Vector3f &cameraPosition, std::vector<int> &triangles)
{
	std::vector<int> visibleTriangles;
	for (int &triangleIdx : triangles)
	{
		Triangle *triangle = mesh->triangles[triangleIdx];
		bool culled = false;
		for (int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
		{
			if (((triangle->vertices[vertexIdx] - cameraPosition) & triangle->normals[vertexIdx]) >= 0)
			{
				culled = true;
				break;
			}
		}
		if (!culled)
		{
			visibleTriangles.push_back(triangleIdx);
		}
	}
	return visibleTriangles;
}

Vector3f Renderer::calculatePhongIlluminationModel(Vector3f &cameraPosition, Vector3f vertexPosition, Vector3f vertexNormal)
{
	if (shadingMode == SHADING_MODE_NONE)
	{
		return getObjectColor();
	}
	vertexNormal = vertexNormal.normalized();
	Vector3f L = (lightPosition - vertexPosition).normalized();
	float distance = (lightPosition - vertexPosition).length();
	float diffuseCoefficient = std::max(0.0f, vertexNormal & L);
	float specularCoefficient = 0.0f;
	if (diffuseCoefficient > 1e-3)
	{
		Vector3f V = (cameraPosition - vertexPosition).normalized();
		Vector3f R = (vertexNormal * 2 * (vertexNormal & L) - L).normalized();
		specularCoefficient = std::max(0.0f, std::pow(R & V, 1.0f - shininess));
		//Vector3f H = (V + L).normalized();
		//specularCoefficient = std::pow(std::max(0.0f, vertexNormal & H), 1.0f - shininess);
	}
	float attenuation = std::min(1.0f / (float)(attenuationConstant + attenuationLinear * distance + attenuationQuadratic * std::pow(distance, 2)), 1.0f);
	float colorR = ambientColor.x + attenuation * lightColor.x / 2 * (objectColor.x * diffuseCoefficient + specularColor.x * specularCoefficient);
	float colorG = ambientColor.y + attenuation * lightColor.y / 2 * (objectColor.y * diffuseCoefficient + specularColor.y * specularCoefficient);
	float colorB = ambientColor.z + attenuation * lightColor.z / 2 * (objectColor.z * diffuseCoefficient + specularColor.z * specularCoefficient);
	/*colorR = std::pow(std::min(colorR, 1.0f), 1.0f / 2.2f);
	colorG = std::pow(std::min(colorG, 1.0f), 1.0f / 2.2f);
	colorB = std::pow(std::min(colorB, 1.0f), 1.0f / 2.2f);*/
	return Vector3f(colorR, colorG, colorB);
}
	
void Renderer::sortVerticesByY(Triangle *triangle, int &minVertex1, int &minVertex2, int &minVertex3)
{
	float y0 = triangle->projectedVertices[0].y;
	float y1 = triangle->projectedVertices[1].y;
	float y2 = triangle->projectedVertices[2].y;
	if (y0 <= y1 && y0 <= y2)
	{
		minVertex1 = 0;
		if (y1 <= y2)
		{
			minVertex2 = 1;
			minVertex3 = 2;
		}
		else
		{
			minVertex2 = 2;
			minVertex3 = 1;
		}
	}
	else if (y1 <= y0 && y1 <= y2)
	{
		minVertex1 = 1;
		if (y0 <= y2)
		{
			minVertex2 = 0;
			minVertex3 = 2;
		}
		else
		{
			minVertex2 = 2;
			minVertex3 = 0;
		}
	}
	else
	{
		minVertex1 = 2;
		if (y0 <= y1)
		{
			minVertex2 = 0;
			minVertex3 = 1;
		}
		else
		{
			minVertex2 = 1;
			minVertex3 = 0;
		}
	}	
}

void Renderer::drawPixel(Buffer *buffer, int x, int y, float z, Vector3f color)
{
	
	if (buffer->getDepth(x, y) > z) // depth buffering
	{
		buffer->setColor(x, y, 0, color.x);
		buffer->setColor(x, y, 1, color.y);
		buffer->setColor(x, y, 2, color.z);
		buffer->setDepth(x, y, z);
	}
}
	
float* Renderer::downsample(float *texture, int w, int h)
{
	int w_ = w >> 1;
	int h_ = h >> 1;
	float *downsampled = new float[w_ * h_ * 3];
	for (int y_ = 0; y_ < h_; y_++)
	{
		int y = y_ << 1;
		for (int x_ = 0; x_ < w_; x_++)
		{
			int x = x_ << 1;
			for (int ch = 0; ch < 3; ch++)
			{
				downsampled[y_ * w_ * 3 + x_ * 3 + ch] = 
					(texture[y * w * 3 + x * 3 + ch] + texture[(y + 1) * w * 3 + x * 3 + ch] + 
					texture[y * w * 3 + (x + 1) * 3 + ch] + texture[(y + 1) * w * 3 + (x + 1) * 3 + ch]) / 4.0f;
			}
		}
	}
	return downsampled;
}

void Renderer::createMipmaps()
{
	mipmapLevels = 0;
	int w = textW, h;
	while (w > 1)
	{
		w >>= 1;
		++mipmapLevels;
	}
	++mipmapLevels;
	textureMipmaps = new float*[mipmapLevels];
	textureMipmaps[0] = new float[textW * textH * 3];
	memcpy(textureMipmaps[0], texture, textW * textH * 3 * sizeof(float));
	w = textW;
	h = textH;
	for (int i = 1; i < mipmapLevels; i++)
	{
		textureMipmaps[i] = downsample(textureMipmaps[i - 1], w, h);
		w >>= 1;
		h >>= 1;
	}
}

Vector3f Renderer::getTextureColor(Vector3f coords)
{
	int x = (int)std::min(coords.x, (float)textW - 1);
	int y = (int)std::min(coords.y, (float)textH - 1);
	float r = texture[y * textW * 3 + x * 3];
	float g = texture[y * textW * 3 + x * 3 + 1];
	float b = texture[y * textW * 3 + x * 3 + 2];
	return Vector3f(r, g, b);
}

Vector3f Renderer::getTextureColorMipmap(Vector3f coords, int level)
{
	int w = textW << level;
	int h = textH << level;
	int x = (int)std::min(coords.x, (float)w);
	int y = (int)std::min(coords.y, (float)h);
	float r = textureMipmaps[level][y * w * 3 + x * 3];
	float g = textureMipmaps[level][y * w * 3 + x * 3 + 1];
	float b = textureMipmaps[level][y * w * 3 + x * 3 + 2];
	return Vector3f(r, g, b);
}
	
Vector3f Renderer::sampleTexture(Vector3f coords, float mipmapLevel)
{
	coords.x *= textW;
	coords.y *= textH;
	if (textureFiltering == TEXTURE_FILTERING_NEAREST_NEIGHBORS)
	{
		float x = std::round(coords.x);
		float y = std::round(coords.y);
		return getTextureColor(Vector3f(x, y, 0.0f));
	}
	else if (textureFiltering == TEXTURE_FILTERING_BILINEAR)
	{
		float x = coords.x;
		float y = coords.y; 
		Vector3f c1 = getTextureColor(Vector3f(std::floor(x), std::floor(y), 0.0f));
		Vector3f c2 = getTextureColor(Vector3f(std::floor(x), std::ceil(y), 0.0f));
		Vector3f c3 = getTextureColor(Vector3f(std::ceil(x), std::floor(y), 0.0f));
		Vector3f c4 = getTextureColor(Vector3f(std::ceil(x), std::ceil(y), 0.0f));
		return (c1 + c2 + c3 + c4) / 4.0f;
	}
	else
	{
		int l1 = (int)mipmapLevel;
		int l2 = l1 + 1;
		float x1 = coords.x / (1 << l1);
		float y1 = coords.y / (1 << l1);
		float x2 = coords.x / (1 << l2);
		float y2 = coords.y / (1 << l2);
		Vector3f c1 = getTextureColorMipmap(Vector3f(x1, y1, 0.0f), l1);
		Vector3f c2 = getTextureColorMipmap(Vector3f(x2, y2, 0.0f), l2);
		float weight1 = 1 - (mipmapLevel - l1);
		float weight2 = 1 - weight1;
		return c1 * weight1 + c2 * weight2;
	}
}

void Renderer::drawLine(Buffer *buffer, int x1, int x2, int y, float z1, float z2, float w1, float w2, Vector3f c1, Vector3f c2, Vector3f t1, Vector3f t2, float mipmapLevel, bool hasTexture, bool wireframe)
{
	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(z1, z2);
		std::swap(w1, w2);
		std::swap(c1, c2);
		std::swap(t1, t2);
	}
	if (wireframe)
	{
		if (hasTexture && texture != nullptr)
		{
			drawPixel(buffer, x1, y, z1, sampleTexture(t1 / w1, mipmapLevel));
			drawPixel(buffer, x2, y, z2, sampleTexture(t2 / w2, mipmapLevel));
		}
		else
		{
			drawPixel(buffer, x1, y, z1, c1 / w1);
			drawPixel(buffer, x2, y, z2, c2 / w2);
		}
	}
	else
	{
		for (int x = x1; x < x2; x++)
		{
			float p = (x - x1) / (float)(x2 - x1);
			float z = z1 * (1 - p) + z2 * p;
			float w = w1 * (1 - p) + w2 * p;
			Vector3f c;
			if (hasTexture && texture != nullptr)
			{
				c = sampleTexture((t1 * (1 - p) + t2 * p) / w, mipmapLevel);
			}
			else
			{
				c = (c1 * (1 - p) + c2 * p) / w;
			}
			drawPixel(buffer, x, y, z, c);
		}
	}
}

void Renderer::rasterizeVertices(Triangle *triangle, Buffer *buffer, bool hasTexture)
{
	for (int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
	{
		int x = (int)triangle->projectedVertices[vertexIdx].x;
		int y = (int)triangle->projectedVertices[vertexIdx].y;
		float z = triangle->projectedVertices[vertexIdx].z;
		Vector3f color;
		if (hasTexture)
		{
			color = sampleTexture(triangle->textureCoords[vertexIdx], 0.0f);
		}
		else
		{
			color = triangle->colors[vertexIdx];
		}
		drawPixel(buffer, x, y, z, color);
	}
}

void Renderer::rasterizeTriangle(Triangle *triangle, Buffer *buffer, bool hasTexture)
{
	int minY1, minY2, minY3;
	sortVerticesByY(triangle, minY1, minY2, minY3);
	Vector3f v1, v2, v3, color1, color2, color3, texture1, texture2, texture3;
	v1 = triangle->projectedVertices[minY1];
	v2 = triangle->projectedVertices[minY2];
	v3 = triangle->projectedVertices[minY3];
	if (perspectiveCorrect)
	{
		color1 = triangle->colors[minY1] / triangle->projectedVertices[minY1].w;
		color2 = triangle->colors[minY2] / triangle->projectedVertices[minY2].w;
		color3 = triangle->colors[minY3] / triangle->projectedVertices[minY3].w;
		texture1 = triangle->textureCoords[minY1] / triangle->projectedVertices[minY1].w;
		texture2 = triangle->textureCoords[minY2] / triangle->projectedVertices[minY2].w;
		texture3 = triangle->textureCoords[minY3] / triangle->projectedVertices[minY3].w;
	}
	else
	{
		color1 = triangle->colors[minY1];
		color2 = triangle->colors[minY2];
		color3 = triangle->colors[minY3];
		texture1 = triangle->textureCoords[minY1];
		texture2 = triangle->textureCoords[minY2];
		texture3 = triangle->textureCoords[minY3];
	}
	float vw1 = 1.0f / triangle->projectedVertices[minY1].w;
	float vw2 = 1.0f / triangle->projectedVertices[minY2].w;
	float vw3 = 1.0f / triangle->projectedVertices[minY3].w;
	float dx = std::max(std::max(std::abs(v1.x - v2.x), std::abs(v1.x - v3.x)), std::abs(v2.x - v3.x));
	float dy = std::max(std::max(std::abs(v1.y - v2.y), std::abs(v1.y - v3.y)), std::abs(v2.y - v3.y));
	float ds = std::max(std::max(std::abs(triangle->textureCoords[minY1].x - triangle->textureCoords[minY2].x), std::abs(triangle->textureCoords[minY1].x - triangle->textureCoords[minY3].x)), std::abs(triangle->textureCoords[minY2].x - triangle->textureCoords[minY3].x)) * textW;
	float dt = std::max(std::max(std::abs(triangle->textureCoords[minY1].y - triangle->textureCoords[minY2].y), std::abs(triangle->textureCoords[minY1].y - triangle->textureCoords[minY3].y)), std::abs(triangle->textureCoords[minY2].y - triangle->textureCoords[minY3].y)) * textH;
	
	float mipmapLevel = std::log2(std::max(std::sqrt(std::pow(ds / dx, 2) + std::pow(dt / dx, 2)), std::sqrt(std::pow(ds / dy, 2) + std::pow(dt / dy, 2))));

	double dx1, dy1, dz1, dw1, incx1, incz1, incw1, dx2, dy2, dz2, dw2, incx2, incz2, incw2, x1, x2, z1, z2, w1, w2;
	Vector3f dc1, incc1, dt1, inct1, dc2, incc2, dt2, inct2, c1, c2, t1, t2;
	
	bool switched = false;
	
	int y = (int)std::ceil(v1.y);
	
	if (v2.y - v1.y < 1e-3) // horizontal line
	{
		dx1 = v1.x - v3.x;
		dy1 = v1.y - v3.y;
		dz1 = v1.z - v3.z;
		dw1 = vw1 - vw3;
		dc1 = color1 - color3;
		dt1 = texture1 - texture3;
		
		dx2 = v2.x - v3.x;
		dy2 = v2.y - v3.y;
		dz2 = v2.z - v3.z;
		dw2 = vw2 - vw3;
		dc2 = color2 - color3;
		dt2 = texture2 - texture3;
			
		incx1 = dx1 / dy1;
		incz1 = dz1 / dy1;
		incw1 = dw1 / dy1;
		incc1 = dc1 / dy1;
		inct1 = dt1 / dy1;
		
		incx2 = dx2 / dy2;
		incz2 = dz2 / dy2;
		incw2 = dw2 / dy2;
		incc2 = dc2 / dy2;
		inct2 = dt2 / dy2;
		
		x1 = v1.x + incx1 * (y - v1.y);
		x2 = v2.x + incx2 * (y - v2.y);
		z1 = v1.z + incz1 * (y - v1.y);
		z2 = v2.z + incz2 * (y - v2.y);
		w1 = vw1 + incw1 * (y - v1.y);
		w2 = vw2 + incw2 * (y - v1.y);
		c1 = color1 + incc1 * (y - v1.y);
		c2 = color2 + incc2 * (y - v2.y);
		t1 = texture1 + inct1 * (y - v1.y);
		t2 = texture2 + inct2 * (y - v2.y);
		
		if (perspectiveCorrect)
		{
			drawLine(buffer, v1.x, v2.x, v1.y, v1.z, v2.z, vw1, vw2, color1, color2, texture1, texture2, mipmapLevel, hasTexture, false);
		}
		else
		{
			drawLine(buffer, v1.x, v2.x, v1.y, v1.z, v2.z, 1.0f, 1.0f, color1, color2, texture1, texture2, mipmapLevel, hasTexture, false);
		}
		
		switched = true;
	}
	else
	{
		dx1 = v2.x - v1.x;
		dy1 = v2.y - v1.y;
		dz1 = v2.z - v1.z;
		dw1 = vw2 - vw1;
		dc1 = color2 - color1;
		dt1 = texture2 - texture1;
		 
		dx2 = v3.x - v1.x;
		dy2 = v3.y - v1.y;
		dz2 = v3.z - v1.z;
		dw2 = vw3 - vw1;
		dc2 = color3 - color1;
		dt2 = texture3 - texture1;
					
		incx1 = dx1 / dy1;
		incz1 = dz1 / dy1;
		incw1 = dw1 / dy1;
		incc1 = dc1 / dy1;
		inct1 = dt1 / dy1;
		
		incx2 = dx2 / dy2;
		incz2 = dz2 / dy2;
		incw2 = dw2 / dy2;
		incc2 = dc2 / dy2;
		inct2 = dt2 / dy2;
		
		x1 = v1.x + incx1 * (y - v1.y);
		x2 = v1.x + incx2 * (y - v1.y);
		z1 = v1.z + incz1 * (y - v1.y);
		z2 = v1.z + incz2 * (y - v1.y);
		w1 = vw1 + incw1 * (y - v1.y);
		w2 = vw1 + incw2 * (y - v1.y);
		c1 = color1 + incc1 * (y - v1.y);
		c2 = color1 + incc2 * (y - v1.y);
		t1 = texture1 + inct1 * (y - v1.y);
		t2 = texture1 + inct2 * (y - v1.y);
	}
	
	while (y < v3.y)
	{
		if (!switched && y > v2.y)
		{
			dx1 = v3.x - v2.x;
			dy1 = v3.y - v2.y;
			dz1 = v3.z - v2.z;
			dw1 = vw3 - vw2;
			dc1 = color3 - color2;
			dt1 = texture3 - texture2;
			incx1 = dx1 / dy1;
			incz1 = dz1 / dy1;
			incw1 = dw1 / dy1;
			incc1 = dc1 / dy1;
			inct1 = dt1 / dy1;
			x1 = v2.x + incx1 * (y - v2.y);
			z1 = v2.z + incz1 * (y - v2.y);
			w1 = vw2 + incw1 * (y - v2.y);
			c1 = color2 + incc1 * (y - v2.y);
			t1 = texture2 + inct1 * (y - v2.y);
			switched = true;
		}
		if (perspectiveCorrect)
		{ 
			drawLine(buffer, (int)x1, (int)x2, (int)y, z1, z2, w1, w2, c1, c2, t1, t2, mipmapLevel, hasTexture, renderMode == RENDER_MODE_WIREFRAME);
		}
		else
		{
			drawLine(buffer, (int)x1, (int)x2, (int)y, z1, z2, 1.0f, 1.0f, c1, c2, t1, t2, mipmapLevel, hasTexture, renderMode == RENDER_MODE_WIREFRAME);
		}
		x1 += incx1;
		x2 += incx2;
		z1 += incz1;
		z2 += incz2;
		w1 += incw1;
		w2 += incw2;
		c1 += incc1;
		c2 += incc2;
		t1 += inct1;
		t2 += inct2;
		++y;
	}
	
	// bottom horizontal line
	if (v3.y - v2.y < 1e-3)
	{
		if (perspectiveCorrect)
		{
			drawLine(buffer, (int)v2.x, (int)v3.x, (int)v2.y, v2.z, v3.z, vw2, vw3, color2, color3, texture2, texture3, mipmapLevel, hasTexture, false);
		}
		else
		{
			drawLine(buffer, (int)v2.x, (int)v3.x, (int)v2.y, v2.z, v3.z, 1.0f, 1.0f, color2, color3, texture2, texture3, mipmapLevel, hasTexture, false);
		}
	}
}

void Renderer::renderClose2GL(Mesh *mesh, Camera &camera, Screen &screen, int w, int h)
{
	if (mesh == nullptr) return;
	Matrix modelView = camera.loadCloseModelViewMatrix();
	Matrix projection = screen.loadCloseProjectionMatrix();
	Matrix viewport = screen.loadCloseViewportMatrix(w, h);
	Matrix vertices = getVertices(mesh);
	Matrix projected = viewport.mult(projection).mult(modelView).mult(vertices);
	Matrix perspectiveDivided = perspectiveDivision(projected);
	std::vector<int> visibleTriangles = clip(perspectiveDivided, w, h);
	//visibleTriangles = backfaceCulling(mesh, camera.origin, visibleTriangles);
	if (visibleTriangles.empty()) return;
	
	Buffer *buffer = new Buffer(w, h);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int &triangleIdx : visibleTriangles)
	{
		Triangle *triangle = mesh->triangles[triangleIdx];
		for (int vertexIdx = 0; vertexIdx < 3; vertexIdx++)
		{
			float x = perspectiveDivided(0, triangleIdx * 3 + vertexIdx);
			float y = perspectiveDivided(1, triangleIdx * 3 + vertexIdx);
			float z = perspectiveDivided(2, triangleIdx * 3 + vertexIdx);
			float w = perspectiveDivided(3, triangleIdx * 3 + vertexIdx);
			Vector3f projectedVertex(x, y, z, w);
			Vector3f color;
			if (shadingMode == SHADING_MODE_FLAT)
			{
				Vector3f triangleCenter = (triangle->vertices[0] + triangle->vertices[1] + triangle->vertices[2]) / 3;
				color = calculatePhongIlluminationModel(camera.origin, triangleCenter, triangle->faceNormal);
			}
			else if (shadingMode == SHADING_MODE_GOURAUD)
			{
				color = calculatePhongIlluminationModel(camera.origin, triangle->vertices[vertexIdx], triangle->normals[vertexIdx]);
			}
			else
			{
				color = getObjectColor();
			}
			triangle->projectedVertices[vertexIdx] = projectedVertex;
			triangle->colors[vertexIdx] = color;
		}
		if (renderMode == RENDER_MODE_POINT)
		{
			rasterizeVertices(triangle, buffer, mesh->texture);
		}
		else
		{
			rasterizeTriangle(triangle, buffer, mesh->texture);
		}
	}
	glEnd();
	glRasterPos2i(0, 0);
	glDrawPixels(w, h, GL_RGB, GL_FLOAT, buffer->color);
	delete buffer;
}	

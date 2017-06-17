#include "Buffer.h"
#include <float.h>

Buffer::Buffer(int w, int h) : w(w), h(h)
{
	color = new float[w * h * 3];
	depth = new float[w * h];
	for (int i = 0, ending = w * h * 3; i < ending; i++)
	{
		color[i] = 0;
	}
	for (int i = 0, ending = w * h; i < ending; i++)
	{
		depth[i] = FLT_MAX;
	}
}

Buffer::~Buffer()
{
	delete[] color;
	delete[] depth;
}


void Buffer::setColor(int x, int y, int ch, float val)
{
	color[y * w * 3 + x * 3 + ch] = val;
	//color[x * h * 3 + y	 * 3 + ch] = val;
}

float Buffer::getColor(int x, int y, int ch)
{
	return color[y * w * 3 + x * 3 + ch];
}

void Buffer::setDepth(int x, int y, float val)
{
	depth[y * w + x] = val;
}

float Buffer::getDepth(int x, int y)
{
	return depth[y * w + x];
}

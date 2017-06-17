#ifndef _BUFFER_H_
#define _BUFFER_H_

class Buffer
{
public:
	float *color;
	float *depth;
	int w;
	int h;
	
	Buffer(int w, int h);
	
	~Buffer();
	
	void setColor(int x, int y, int ch, float val);
	
	float getColor(int x, int y, int ch);
	
	void setDepth(int x, int y, float val);
	
	float getDepth(int x, int y);
	
};

#endif // _BUFFER_H_

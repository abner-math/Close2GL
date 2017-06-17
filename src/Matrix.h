#ifndef _MATRIX_H_
#define _MATRIX_H_

class Matrix
{
public:
	int rows, cols;
	float *data;

	Matrix(int rows = 0, int cols = 0);
	
	Matrix(const Matrix &other);
	
	~Matrix();
	
	Matrix mult(Matrix &other);
	
	Matrix transposed();
	
	Matrix& operator=(const Matrix &other);
	
	float& operator()(int i, int j)
	{
		return data[i * cols + j];
	}
	
};

#endif // _MATRIX_H_

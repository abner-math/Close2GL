#include "Matrix.h"
#include <cstring>

Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols)
{
	if (rows == 0 || cols == 0) data = nullptr;
	else data = new float[rows * cols];
}

Matrix::Matrix(const Matrix &other) 
{
	rows = other.rows;
	cols = other.cols;
	if (rows == 0 || cols == 0)
		data = nullptr;
	else {
		data = new float[rows * cols];
		std::memcpy(data, other.data, rows*cols*sizeof(float));
	}
}

Matrix::~Matrix()
{
	if (data != nullptr)
		delete[] data;
}

Matrix Matrix::mult(Matrix &other)
{
	Matrix res(rows, other.cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < other.cols; j++)
		{
			float sum = 0;
			for (int k = 0; k < other.rows; k++)
			{
				sum += (*this)(i, k) * other(k, j);
			}
			res(i, j) = sum;
		}
	}
	return res;
}

Matrix Matrix::transposed()
{
	Matrix trans(cols, rows);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			trans(j, i) = (*this)(i, j);
		}
	}
	return trans;
}	

Matrix& Matrix::operator=(const Matrix &other)
{
	rows = other.rows;
	cols = other.cols;
	delete[] data;
	data = new float[rows * cols];
	std::memcpy(data, other.data, rows*cols*sizeof(float));
}	

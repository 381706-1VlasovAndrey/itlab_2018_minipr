#include <iostream>
#include "omp.h"

int main()
{
	int n;
	std::cin >> n;
	double *MatrixA = new double[n*n];
	double *MatrixB = new double[n*n];
	double *MatrixC = new double[n*n];
	for (int i = 0; i < n; i++) 
		for (int j = 0; j < n; j++) 
	{
			MatrixA[i * n + j] = rand() % 10;
			MatrixB[i * n + j] = rand() % 10;
	}
	
    #pragma omp parallel for
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				for (int k = 0; k < n; k++)
				{
					MatrixC[i * n + k] += MatrixA[i * n + j] * MatrixB[j * n + k];
				}
			}
		}
		return 0;
}

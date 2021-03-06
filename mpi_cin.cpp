#include "pch.h"
#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
	
int main(int argc, char* argv[])
{
	// Умножение матрицы на вектор 
	int procs_rank, procs_count;
	int i, j, n , local_n;
	std::cin >> n;
	i = j = n;
	int *local_a, *b, *local_c, *a, *c;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procs_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
	local_n = n / procs_count;
	local_a = new int[local_n * n];
	b = new int[n];
	local_c = new int[local_n];
	c = new int[n];
	a = new int[n * n];
	//инициализируем матрицу в нулевом процессе 
	// и рассылаем его части по процессам 
	if (procs_rank == 0)
	{
		//матрицу удобнее рассматривать как одномерный массив но с индексами i*n+j 
		std::cout << "cin a\n";
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				std::cin >> a[i*n + j];
		std::cout << "cin b\n";
		for (i = 0; i < n; i++)
			std::cin >> b[i];
	}
	//рассылаем вектор b 
	MPI_Bcast(b, n, MPI_INT, 0, MPI_COMM_WORLD);
	//разделяем матрицу на горизонтальные полосы 
	//шириной local_a и отпраляем процессам 
	MPI_Scatter(a, n * local_n, MPI_INT, local_a, n * local_n, MPI_INT, 0, MPI_COMM_WORLD);

	for (i = 0; i < local_n; i++) {
		local_c[i] = 0;
		for (j = 0; j < n; j++)
			local_c[i] += local_a[i*n + j] * b[j];
	}
	//собираем результат в нулевом процессе 
	MPI_Gather(local_c, local_n, MPI_INT, c, local_n, MPI_INT, 0, MPI_COMM_WORLD);
	if (procs_rank == 0)
	{
		std::cout << "c = ";
		for (i = 0; i < n; i++)
			std::cout<< "\n" << c[i];
	}
	MPI_Finalize();
	return 0;
}


#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
	
int main(int argc, char* argv[])
{
	// Умножение матрицы на вектор 
	int procs_rank, procs_count;
	int i, j, n , local_n;
	std::cout << "cin n\n";
	std::cin >> n;
	std::cout << "cin local_n\n";
	std::cin >> local_n;
	i = j = n;
	int *local_a, *b, *local_c, *a, *c;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procs_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &procs_rank);
	MPI_Status status;
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
	//шириной local_n и отпраляем процессам,
	//вычисляем, собираем результат и заново
	for (i = 0; i < (n - (n%((procs_count-1)*local_n))); i += (procs_count-1)*local_n) {
		for(j = 1; j < procs_count; j++)
		MPI_Send(a + n * (i + (j-1)*local_n), n*local_n, MPI_INT, j, 0, MPI_COMM_WORLD);
		for (int k = 0; k < local_n; k++) {
			local_c[k] = 0;
			for (int p = 0; p < n; p++)
				local_c[k] += local_a[k*n + p] * b[p];
		} 
		MPI_Gather(local_c, local_n, MPI_INT, c + i, local_n, MPI_INT, 0, MPI_COMM_WORLD);
	}
	if ( n % ((procs_count - 1)*local_n) != 0) {
		MPI_Send(a + n * (i - (procs_count - 1)*local_n), n * (n - i - (procs_count - 1)*local_n - 1), MPI_INT, 1, 0, MPI_COMM_WORLD);
		for (int k = 0; k < n % ((procs_count - 1)*local_n); k++) {
			local_c[k] = 0;
			for (int p = 0; p < n; p++)
				local_c[k] += local_a[k*n + p] * b[p];
		}
		MPI_Recv(local_c, n % ((procs_count - 1)*local_n), MPI_INT, 1, 0, MPI_COMM_WORLD, & status);
	}
	if (procs_rank == 0)
	{
		std::cout << "c = ";
		for (i = 0; i < n; i++)
			std::cout<< "\n" << c[i];
	}
	MPI_Finalize();
	return 0;
}


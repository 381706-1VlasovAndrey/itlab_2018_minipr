#include <iostream>
#include <cmath>
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_sort.h"
#include "tbb/tick_count.h"

void QuickSort(int* a, const int n) {
  int i = 0, j = n - 1;
  int pivot = a[n / 2];
  do {
	while (a[i] < pivot) i++;
	while (a[j] > pivot) j--;
	if (i <= j) {
	  std::swap(a[i], a[j]);
	  i++; j--;
	}
  } while (i <= j);
  if (j > 0) QuickSort(a, j);
  if (n > i) QuickSort(a + i, n - i);
}

class TBB_QuickSort :public tbb::task
{
  int n;
  int* a;
public:
  TBB_QuickSort(int *_a, int _n) : a(_a), n(_n) {}
  task* execute()
  {
	int i = 0, j = n, pivot = a[n / 2];
	if ((j - i) >= 0 && (j - i) < 100)
	{
	  for (int k = i; k <= j; k++)
		for (int l = k + 1; l <= j; l++)
		  if (a[k] > a[l])
			std::swap(a[k], a[l]);
	  return 0;
	}
	do {
	  while (a[i] < pivot) i++;
	  while (a[j] > pivot) j--;
	  if (i <= j) {
		std::swap(a[i], a[j]);
		i++; j--;
	  }
	} while (i <= j);
	if (j > 0)
	{
	  tbb::empty_task& c = *new(allocate_continuation()) tbb::empty_task;
	  TBB_QuickSort &t1 = *new(c.allocate_child()) TBB_QuickSort(a, j);
	  c.set_ref_count(c.ref_count() + 1);
	  c.spawn_and_wait_for_all(t1);
	}
	if (n > i)
	{
	  tbb::empty_task& c = *new(allocate_continuation()) tbb::empty_task;
	  TBB_QuickSort &t2 = *new(c.allocate_child()) TBB_QuickSort(a + i, n - i);
	  c.set_ref_count(c.ref_count() + 1);
	  c.spawn_and_wait_for_all(t2);
	}
	return 0;
  }
};


int main(int argc, char** argv)
{
  int *a, *b;
  //int n = atoi(argv[1]);
  int n = 1000000;
  a = new int[n];
  b = new int[n];
  for (int i = 0; i < n; ++i) {
	a[i] = rand() % 100;
	b[i] = a[i];
  }

  tbb::task_scheduler_init init;

  tbb::tick_count t1 = tbb::tick_count::now();

  TBB_QuickSort &q = *new(tbb::task::allocate_root()) TBB_QuickSort(a, n);
  tbb::task::spawn_root_and_wait(q);

  tbb::tick_count t2 = tbb::tick_count::now();

  QuickSort(b, n);
  tbb::tick_count t3 = tbb::tick_count::now();

  for (int i = 0; i < n; i++)
  {
	if (a[i] != b[i])
	{
	  std::cout << "Error!";
	  return 0;
	}
  }

  std::cout << (t2 - t1).seconds() << " " << (t3 - t2).seconds() << "\n";

  delete[] a, b;
  return 0;
}

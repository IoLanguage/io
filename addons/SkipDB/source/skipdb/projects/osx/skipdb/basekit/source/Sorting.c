#include "Sorting.h"

typedef struct
{
	void *context;
	SDSortCompareCallback *comp; 
	SDSortSwapCallback *swap;
} SDSort;

int Sorting_isSorted(SDSort *self, int size);
void Sorting_quickSort(SDSort *self, int lb, int ub);
int Sorting_quickSortRearrange(SDSort *self, int lb, int ub);
//void Sorting_shellSort(SDSort *self, int size);

void Sorting_context_comp_swap_size_type_(void *context, 
									SDSortCompareCallback *comp, 
									SDSortSwapCallback *swap, 
									int size, 
									SDSortType type)
{
	SDSort q;
	SDSort *self = &q;
	
	self->context = context;
	self->comp = comp;
	self->swap = swap;
	
	switch (type)
	{
		case SDQuickSort:
			if (!Sorting_isSorted(self, size)) Sorting_quickSort(self, 0, size-1);
			break;
		/*
		case SDShellSort:
			if (!Sorting_isSorted(self, size)) Sorting_shellSort(self, size);
			break;
		*/
	}
}

int Sorting_isSorted(SDSort *self, int size)
{
	SDSortCompareCallback *comp = self->comp;
	void *context = self->context;
	int i;
	
	for (i = 0; i < size - 2; i ++)
	{
		if ((*comp)(context, i, i + 1) > 0) 
		{
			return 0;  
		}
	}
	
	return 1;
}

void Sorting_quickSort(SDSort *self, int lb, int ub)
{
	if (lb < ub) 
	{
		int j = Sorting_quickSortRearrange(self, lb,ub);
		
		if (j) 
		{
			Sorting_quickSort(self, lb, j - 1);
		}
		
		Sorting_quickSort(self, j + 1, ub);
	}
}

int Sorting_quickSortRearrange(SDSort *self, int lb, int ub)
{
	SDSortCompareCallback *comp = self->comp;
	SDSortSwapCallback *swap = self->swap;
	void *context = self->context;
	
	do {
		while (ub > lb && (*comp)(context, ub, lb) >= 0)
		{
			ub --;
		}
		
		if (ub != lb) 
		{
			(*swap)(context, ub, lb);
			
			while (lb < ub && (*comp)(context, lb, ub) <= 0)
			{ 
				lb ++; 
			}
			
			if (lb != ub)
			{
				(*swap)(context, lb, ub);
			}
		}
	} while (lb != ub);
	
	return lb;
}


/*
void Sorting_shellSort(SDSort *self, int size)
{
	SDSortCompareCallback *comp = self->comp;
	SDSortSwapCallback *swap = self->swap;
	void *context = self->context;
	
	int m = size;
	
	while (m /= 2) 
	{
		int k = size - m;
		int j = 1;
		
		do 
		{
			int i = j;
			
			do 
			{
				int h = i + m;
				
				if ((*comp)(context, i - 1, h - 1) > 0) 
				{
					(*swap)(context, i - 1, h - 1);
					i -= m;
				}
				else
				{ 
					break; 
				}
				
			} while (i >= 1);
			
			j += 1;
			
		} while(j <= k);
	}
}
*/

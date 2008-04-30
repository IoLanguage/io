#include "PortableSorting.h"


typedef struct
{
	void *base;
	size_t size;
	size_t width;
	void *context;
	PortableSortingCompareCallback *compare;
} Sorter;

void Sorter_quickSort(Sorter *self, size_t lb, size_t ub);
int Sorter_quickSortRearrange(Sorter *self, size_t lb, size_t ub);

void portable_qsort_r(void *base, size_t size, size_t width, 
	void *context, PortableSortingCompareCallback *compare)
{
	Sorter s;
	s.base = base;
	s.size = size;
	s.width = width;
	s.context = context;
	s.compare = compare;
	Sorter_quickSort(&s, 0, size-1);
}

void Sorter_quickSort(Sorter *self, size_t lb, size_t ub)
{
	if (lb < ub)
	{
		size_t j = Sorter_quickSortRearrange(self, lb, ub);

		if (j)
		{
			Sorter_quickSort(self, lb, j - 1);
		}

		Sorter_quickSort(self, j + 1, ub);
	}
}

int Sorter_quickSortRearrange(Sorter *self, size_t lb, size_t ub)
{
	PortableSortingCompareCallback *comp = self->compare;
	void *context = self->context;
	void *base = self->base;
	size_t width = self->width;

	do {
		while (ub > lb && (*comp)(context, ub, lb) >= 0)
		{
			ub --;
		}

		if (ub != lb)
		{
			memcpy(((unsigned char *)base) + width*ub, ((unsigned char *)base) + width*lb, width);

			while (lb < ub && (*comp)(context, lb, ub) <= 0)
			{
				lb ++;
			}

			if (lb != ub)
			{
				memcpy(((unsigned char *)base) + width*lb, ((unsigned char *)base) + width*ub, width);
			}
		}
	} while (lb != ub);

	return lb;
}

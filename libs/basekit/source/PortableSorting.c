#include "PortableSorting.h"

typedef struct
{
	void *base;
	size_t size;
	size_t width;
	void *context;
	PortableSortingCompareCallback compare;
	unsigned char *swapSpace;
} Sorter;

void Sorter_quickSort(Sorter *self, size_t lb, size_t ub);
size_t Sorter_quickSortRearrange(Sorter *self, size_t lb, size_t ub);

void portable_qsort_r(void *base, size_t size, size_t width, 
	void *context, PortableSortingCompareCallback compare)
{
	if (size > 0 && width > 0)
	{
		Sorter s;
		s.base = base;
		s.size = size;
		s.width = width;
		s.context = context;
		s.compare = compare;
		s.swapSpace = malloc(width);
		Sorter_quickSort(&s, 0, size-1);
		free(s.swapSpace);
	}
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

static void swap(void *base, size_t a, size_t b, size_t width, unsigned char *swapSpace)
{
	//unsigned char swapSpace[width]; // windows can't deal with this
	void *ap = ((unsigned char *)base) + width*a;
	void *bp = ((unsigned char *)base) + width*b;
	memcpy(swapSpace, ap, width);
	memcpy(ap, bp, width);
	memcpy(bp, swapSpace, width);
}

size_t Sorter_quickSortRearrange(Sorter *self, size_t lb, size_t ub)
{
	PortableSortingCompareCallback comp = self->compare;
	void *context = self->context;
	unsigned char *base = self->base;
	size_t width = self->width;
	
	do {
		while (ub > lb && (*comp)(context, base + width*ub, base + width*lb) >= 0)
		{
			ub --;
		}

		if (ub != lb)
		{
			swap(base, ub, lb, width, self->swapSpace);
			
			while (lb < ub && (*comp)(context, base + width*lb, base + width*ub) <= 0)
			{
				lb ++;
			}

			if (lb != ub)
			{
				swap(base, lb, ub, width, self->swapSpace);
			}
		}
	} while (lb != ub);

	return lb;
}


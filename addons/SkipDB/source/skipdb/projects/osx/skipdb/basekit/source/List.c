/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#define LIST_C
#include "List.h"
#undef LIST_C
#include <stdlib.h>

List *List_new(void)
{
    List *self = (List *)calloc(1, sizeof(List));
    self->size = 0;
    self->memSize = sizeof(void *)*LIST_START_SIZE;
    self->items = (void **)calloc(1, self->memSize);
    return self;
}

List *List_clone(List *self)
{
    List *child = List_new();
    List_copy_(child, self);
	/*
	List *child = cpalloc(self, sizeof(List));
	child->items = cpalloc(self->items, self->memSize);
	*/
    return child;
}

static size_t indexWrap(int index, size_t size)
{
    if (index < 0)
    {
		index = size - index;
		
		if (index < 0)
		{
			index = 0;
		}
    }
    
    if (index > (int)size)
    {
		index = size;
    }
    
    return (size_t)index;
}

void List_sliceInPlace(List* self, int startIndex, int endIndex)
{
    size_t i, size = List_size(self);
    List *tmp = List_new();
    size_t start = indexWrap(startIndex, size);
    size_t end   = indexWrap(endIndex, size);
    
    for (i = start; i < size && i < end + 1; i ++)
    {
		List_append_(tmp, List_at_(self, i));
    }
    
    List_copy_(self, tmp);
    List_free(tmp);
}

List *List_cloneSlice(List *self, int startIndex, int endIndex)
{
    List *child = List_clone(self);
    List_sliceInPlace(child, startIndex, endIndex);
    return child;
}

void List_free(List *self)
{
	//printf("List_free(%p)\n", (void *)self);
	free(self->items);
	free(self);
}

size_t List_memorySize(List *self)
{
    return sizeof(List) + (self->size * sizeof(void *));
}

void List_removeAll(List *self) 
{ 
    self->size = 0; 
	List_compactIfNeeded(self);
}

void List_copy_(List *self, List *otherList)
{
    if (self == otherList || (!otherList->size && !self->size)) 
    { 
		return; 
    }
    
    List_preallocateToSize_(self, otherList->size);
    memmove(self->items, otherList->items, sizeof(void *) * (otherList->size));
    self->size = otherList->size;
}

int List_equals_(List *self, List *otherList)
{
    return (self->size == otherList->size &&
			memcmp(self->items, otherList->items, sizeof(void *) * self->size) == 0);
}

/* --- sizing ------------------------------------------------ */

void List_setSize_(List *self, size_t index)
{ 
    List_ifNeededSizeTo_(self, index);
    self->size = index; 
}

void List_preallocateToSize_(List *self, size_t index)
{
    size_t s = index * sizeof(void *);
    
    if (s >= self->memSize) 
    {
		size_t newSize = self->memSize * LIST_RESIZE_FACTOR;
		
		if (s > newSize) 
		{ 
			newSize = s; 
		}
		
		self->items = (void **)realloc(self->items, newSize); 
		memset(self->items + self->size, 0, (newSize - (self->size*sizeof(void *))));
		self->memSize = newSize;
    }
}

void List_compact(List *self)
{
    self->memSize = self->size * sizeof(void *);
    self->items = (void **)realloc(self->items, self->memSize); 
}

/* ----------------------------------------------------------- */

void List_removeItemsAfterLastNULL_(List *self)
{
    long i;
    void **items = self->items;
    
    for (i = self->size - 1; i > -1; i --) 
    { 
		if (!items[i]) 
		{ 
			break; 
		} 
    }
    
    self->size = i;
	List_compactIfNeeded(self);
}

void List_print(List *self)
{
    int i;
    
    printf("List <%p> [%i bytes]\n", (void *)self, (int)self->memSize);
    
    for (i = 0; i < self->size; i ++)
    { 
		printf("%i: %p\n", i, (void *)self->items[i]); 
    }
    
    printf("\n");
}

/* --- perform -------------------------------------------------- */

void List_target_do_(List *self, void *target, ListDoWithCallback *callback)
{
    size_t i, count = self->size;
    void **items = self->items;
    
    for (i = 0; i < count; i ++) 
    { 
		void *item = items[i];
		
		if (item) 
		{
			(*callback)(target, item); 
		}
    }
}

void List_do_(List *self, ListDoCallback *callback)
{
    size_t i, count = self->size;
    void **items = self->items;
    
    for (i = 0; i < count; i ++) 
    { 
		void *item = items[i];
		
		if (item) 
		{
			(*callback)(item); 
		}
    }
}

void List_do_with_(List *self, ListDoWithCallback *callback, void *arg)
{
    size_t i, count = self->size;
    void **items = self->items;
    
    for (i = 0; i < count; i ++) 
    { 
		void *item = items[i];
		
		if (item) 
		{
			(*callback)(item, arg); 
		}
    }
}

void List_mapInPlace_(List *self, ListCollectCallback *callback)
{
    size_t i, count = self->size;
    void **items = self->items;
    
    for (i = 0; i < count; i ++) 
    { 
		void *item = items[i];
		
		items[i] = (*callback)(item);
    }
}

List *List_map_(List *self, ListCollectCallback *callback)
{
    List *results = List_new();
    size_t i, count = self->size;
    void **items = self->items;
    
    for (i = 0; i < count; i ++) 
    { 
		void *item = items[i];
		void *result = (*callback)(item);
		
		List_append_(results, result);
    }
    
    return results;
}

List *List_select_(List *self, ListSelectCallback *callback)
{
    List *results = List_new();
    size_t i, count = self->size;
    void **items = self->items;
    
    for (i = 0; i < count; i ++) 
    { 
		void *item = items[i];
		
		if (item && (*callback)(item)) 
		{ 
			if (item) List_append_(results, item);
		}
    }
    
    return results;
}

void *List_detect_(List *self, ListDetectCallback *callback)
{
    size_t i, count = self->size;
    void **items = self->items;
    
    for (i = 0; i < count; i ++) 
    { 
		void *item = items[i];
		
		if (item && (*callback)(item)) 
		{
			return item;
		}
    }
    
    return (void *)NULL;
}

/*
 void *List_detect_withArg_(List *self, ListDetectCallback *callback, void *arg)
 {
     int i, count = self->size;
     void **items = self->items;
     
     for (i = 0; i < count; i ++) 
     { 
		 void *item = items[i];
		 if (item && (*callback)(item, arg)) 
		 {
			 return item;
		 }
     }
     
     return (void *)NULL;
 }
 */

void *List_anyOne(List *self)
{ 
    size_t i;
    
    if (self->size == 0) 
    {
		return (void *)NULL;
    }
    
    if (self->size == 1) 
    {
		return LIST_AT_(self, 0);
    }
    
    i = (rand() >> 4) % (self->size); // without the shift, just get a sequence! 
    
    return LIST_AT_(self, i);
}

void List_shuffle(List *self)
{ 
    size_t i, j;
    
    for (i = 0; i < self->size - 1; i ++)
    {
		j = i + rand() % (self->size - i); 
		List_swap_with_(self, i, j);
    }
}

void *List_removeLast(List *self)
{
    void *item = List_at_(self, self->size - 1);
    
    if (item) 
    {
		self->size --;
    	List_compactIfNeeded(self);
	}
	    
    return item;
}

void List_append_sortedBy_(List *self, void *item, ListSortCallback *callback)
{
    // sort lowest to highest 
    
    size_t i;
    
    for (i = 0; i < self->size - 1; i ++)
    {
		void *other = List_at_(self, i);
		
		if ((*callback)(item, other) < 0)
		{ 
			List_at_insert_(self, i, item); 
			return; 
		}
    }
    
    List_append_(self, item);
}

/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#define PHASH_C
#include "PHash.h"
#undef PHASH_C
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PHASH_DEFAULT_SIZE 2  

PHash *PHash_new(void) 
{
	PHash *self = (PHash *)calloc(1, sizeof(PHash)); 
	self->tableSize = PHASH_DEFAULT_SIZE;
	self->records = (PHashRecord *)calloc(1, sizeof(PHashRecord) * PHASH_DEFAULT_SIZE);
	self->numKeys = 0;
	return self; 
}

PHash *PHash_clone(PHash *self)
{
	PHash *child = PHash_new();
	PHash_copy_(child, self);
	return child;
}

void PHash_free(PHash *self)
{
	free(self->records);
	free(self);
}

unsigned int PHash_size(PHash *self) 
{ 
	return self->tableSize; 
}

size_t PHash_memorySize(PHash *self)
{ 
	return sizeof(PHash) + (self->tableSize * sizeof(PHashRecord));
}

void PHash_compact(PHash *self)
{ 
	printf("need to implement PHash_compact\n"); 
}

void PHash_copy_(PHash *self, PHash *other)
{
	PHashRecord *records = self->records;
	memcpy(self, other, sizeof(PHash));
	self->records = (PHashRecord *)realloc(records, sizeof(PHashRecord) * self->tableSize);
	memcpy(self->records, other->records, sizeof(PHashRecord) * self->tableSize);
}

void PHash_collapseRecordsAndAddKey_value_(PHash *self, void *key, void *value)
{
	// collapse the records into a key list 
	// so we can enumerate them quickly while rehashing 
	
	PHashRecord *records = self->records;
	size_t i, j = 0;
	
	for (i = 0; i < self->tableSize; i ++)
	{
		if (records[i].key)
		{
			memmove(records + j, records + i, sizeof(PHashRecord));
			j ++;
		}
	}
	
	if (j == self->tableSize) // if needed, add room for 1 more 
	{ 
		PHash_setTableSize_(self, self->tableSize + 1);
		records = self->records;
	}
	
	// add the new record to the end of the list 
	records[j].key   = key;
	records[j].value = value;
	self->numKeys ++;
}

static void PHash_clearTable(PHash *self)
{
	memset(self->records, 0, sizeof(PHashRecord) * self->tableSize);
}

void PHash_rehashWithCollapsedRecords(PHash *self)
{
	register PHashRecord *collapsedRecords = self->records;
	register unsigned int i, max;
	
#ifdef PHASH_USE_CACHE
	self->cachedLookup = 0;
#endif
	
	self->records = NULL;
	PHash_prepareRehash(self);
	
collision:
		PHash_nextRehash(self);
	PHash_clearTable(self);
	max = self->numKeys;
	
	for (i = 0; i < max; i ++)
	{
		void *key = collapsedRecords[i].key;
		unsigned int index = PHash_hash(self, key);
		/*
		 if (key = NULL)
		 {
			 printf("ERROR: NULL key\n");
			 exit(1);
		 }
		 
		 if (index > self->tableSize - 1)
		 {
			 printf("ERROR: index %i > tableSize %i\n", index, self->tableSize);
			 exit(1);
		 }
		 */
		
		if (self->records[index].key) 
		{
			goto collision;
		}
		
		memcpy(self->records + index, collapsedRecords + i, sizeof(PHashRecord));
	}
	
	free(collapsedRecords);
	//printf("rehashed: %p  %i\t %i\n", self, self->numKeys, self->tableSize);
}

void *PHash_firstValue(PHash *self)
{
	self->index = 0;
	return PHash_nextValue(self);
}

void *PHash_nextValue(PHash *self)
{
	PHashRecord *record;
	
	while (self->index < self->tableSize)
	{
		record = PHASH_RECORDAT_(self, self->index);
		self->index++;
		
		if (record->key) 
		{
			return record->value;
		}
	}
	
	return (void *)NULL;
}

void PHash_removeValue_(PHash *self, void *value)
{
	PHashRecord *record;
	int index = 0;
	
	while (index < self->tableSize)
	{
		record = PHASH_RECORDAT_(self, index);
		index ++;
		
		if (record->key && record->value == value)
		{
			self->numKeys --;
			memset(record, 0, sizeof(PHashRecord));
			return;
		}
	}
}

void *PHash_firstKey(PHash *self)
{
	self->index = 0;
	return PHash_nextKey(self);
}

void *PHash_nextKey(PHash *self)
{
	PHashRecord *record;
	
	while (self->index < self->tableSize)
	{
		record = PHASH_RECORDAT_(self, self->index);
		self->index ++;
		
		if (record->key) 
		{
			return record->key;
		}
	}
	
	return (void *)NULL;
}

/*
 unsigned int PHash_countRecords_size_(unsigned char *newRecords, unsigned int size)
 {
	 unsigned int m;
	 unsigned int c = 0;
	 
	 for (m = 0 ; m < size; m ++)
	 {
		 PHashRecord *r = (PHashRecord *)(newRecords + m);
		 
		 if (r->key) 
		 { 
			 c++; 
		 }
	 }
	 return c;
 }
 */

unsigned int PHash_doCount(PHash *self)
{
	unsigned int m;
	unsigned int c = 0;
	
	for (m = 0 ; m < self->tableSize; m++)
	{
		PHashRecord *r = self->records + m;
		
		if (r->key) 
		{ 
			c++; 
		}
	}
	
	return c;
}

PHashRecord *PHash_recordAtIndex_(PHash *self, unsigned int index)
{
	unsigned int i = 0;
	unsigned int count = 0;
	
	while (i < self->tableSize)
	{
		PHashRecord *record = PHASH_RECORDAT_(self, i);
		
		if (record->key) 
		{
			if (index == count) 
			{ 
				return record; 
			}
			count++;
		}
		i ++;
	}
	
	return (PHashRecord *)NULL;
}

void *PHash_keyAt_(PHash *self, unsigned int i)
{
	PHashRecord *record = PHash_recordAtIndex_(self, i);
	return (record) ? record->key : (void *)NULL;
}

void *PHash_valueAt_(PHash *self, unsigned int i)
{
	PHashRecord *record = PHash_recordAtIndex_(self, i);
	return (record) ? record->value : (void *)NULL;
}

int PHash_indexForValue_(PHash *self, void *v)
{
	unsigned int i = 0;
	unsigned int count = 0;
	
	while (i < self->tableSize)
	{
		PHashRecord *record = PHASH_RECORDAT_(self, i);
		
		if (record->key)
		{
			if (record->value == v) 
			{ 
				return count; 
			}
			
			count++;
		}
		i ++;
	}
	
	return -1;
}

void *PHash_firstKeyForValue_(PHash *self, void *v)
{
	unsigned int i = 0;
	unsigned int count = 0;
	
	while (i < self->tableSize)
	{
		PHashRecord *record = PHASH_RECORDAT_(self, i);
		
		if (record->key)
		{
			if (record->value == v) 
			{ 
				return record->key; 
			}
			count++;
		}
		i ++;
	}
	return NULL;
}

// --- enumeration -------------------------------------------------- 

void PHash_do_(PHash *self, PHashDoCallback *callback)
{
	unsigned int i, tableSize = self->tableSize;
	
	for (i = 0; i < tableSize; i ++)
	{
		PHashRecord *record = PHASH_RECORDAT_(self, i);
		
		if (record->key) 
		{ 
			(*callback)(record->value); 
		}
	}
}

void *PHash_detect_(PHash *self, PHashDetectCallback *callback)
{
	unsigned int i, tableSize = self->tableSize;
	
	for (i = 0; i < tableSize; i ++)
	{
		PHashRecord *record = PHASH_RECORDAT_(self, i);
		
		if (record->key) 
		{ 
			if ((*callback)(record->value))
			{
				return record->key;
			}
		}
	}
	
	return NULL;
}

void PHash_doOnKeys_(PHash *self, PHashDoCallback *callback)
{
	unsigned int i, tableSize = self->tableSize;
	
	for (i = 0; i < tableSize; i ++)
	{
		PHashRecord *record = PHASH_RECORDAT_(self, i);
		
		if (record->key) 
		{ 
			(*callback)(record->key); 
		}
	}
}

/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#define HASH_C
#include "Hash.h"
#undef HASH_C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef LOW_MEMORY_SYSTEM
#define HASH_DEFAULT_SIZE 128
#define HASH_RESIZE_FACTOR 1.2
#define HASH_COUNT_TO_SIZE_RATIO 5
#else
#define HASH_DEFAULT_SIZE 128
#define HASH_RESIZE_FACTOR 1.2
#define HASH_COUNT_TO_SIZE_RATIO 10
#endif

// should use bit shifting and power of 2 sizes instead 
#define HASHFUNCTION(v, tsize) (unsigned int)((ptrdiff_t)v)%(tsize-1)

Hash *Hash_new(void)
{
	Hash *self = (Hash *)calloc(1, sizeof(Hash));
	self->size = HASH_DEFAULT_SIZE;
	self->records = (HashRecord **)calloc(1, sizeof(HashRecord *) * self->size);
	self->count = 0;
	self->first = (HashRecord *)NULL;
	return self;
}

Hash *Hash_clone(Hash *self)
{
	Hash *child = Hash_new();
	Hash_copy_(child, self);
	return child;
}

void Hash_copy_(Hash *self, Hash *other)
{
	HashRecord *record = other->first;
	
	while (record)
	{
		Hash_at_put_(self, record->key, record->value);
		record = record->nextRecord;
	}
}

void Hash_free(Hash *self)
{
	Hash_freeRecords(self);
	free(self->records);
	free(self);
}

void Hash_freeRecords(Hash *self)
{
	HashRecord *record = self->first;
	
	while (record)
	{
		HashRecord *nextRecord = record->nextRecord;
		HashRecord_free(record);
		record = nextRecord;
	}
}

void Hash_clean(Hash *self)
{
	Hash_freeRecords(self);
	self->first = (HashRecord *)NULL;
	self->count = 0;
	memset(self->records, (unsigned char)0, sizeof(HashRecord *) * self->size);
}

void Hash_rehash(Hash *self)
{
	HashRecord *record = self->first;
	
	//printf("rehash\n");
	
	// clear old bucket links 
	
	while (record)
	{
		HashRecord_next_(record, (HashRecord *)NULL);
		record = record->nextRecord;
	}
	
	self->size = (size_t)(self->size * HASH_RESIZE_FACTOR) + 1;
	self->records = (HashRecord **)realloc(self->records, sizeof(HashRecord *) * self->size);
	memset(self->records, (unsigned char)0, sizeof(HashRecord *) * self->size);
	
	record = self->first;
	
	while (record)
	{
		unsigned int hval = HASHFUNCTION(record->key, self->size);
		HashRecord *r = self->records[hval];
		
		self->records[hval] = record;
		
		if (r)
		{
			HashRecord_next_(record, r);
		}
		
		record = record->nextRecord;
	}
	
	/*
	 printf("rehased to size:%i count:%i ratio:%f\n", 
		   self->size, self->count, (float)self->size/(float)self->count);
	 Hash_verify(self);
	 */
}

void *Hash_at_(Hash *self, void *key)
{
	unsigned int hval = HASHFUNCTION(key, self->size);
	HashRecord *record = self->records[hval];
	
	if (record)
	{
		if (record->key == key) 
		{ 
			return record->value; 
		}
		
		{
			HashRecord *lastRecord = record;
			record = record->next;
			
			while (record)
			{
				if (record->key == key) 
				{ 
					/* move to front of bucket */
					HashRecord_next_(lastRecord, record->next);
					HashRecord_next_(record, self->records[hval]);
					self->records[hval] = record;
					return record->value; 
				}
				
				lastRecord = record;
				record = record->next;
			}
		}
	}
	//Hash_verify(self);
	return (void *)NULL;
}

void Hash_at_put_(Hash *self, void *key, void *value)
{
	unsigned int hval = HASHFUNCTION(key, self->size);
	HashRecord *record = self->records[hval];
	HashRecord *lastRecord = record; // for testing
	
	while (record)
	{
		if (record->key == key) 
		{ 
			record->value = value; // dont increment count
			return; 
		}
		
		lastRecord = record; // for testing
		record = record->next;
	}
	
	/* no match, add new hash record */
	
	record = HashRecord_newWithKey_value_(key, value);
	
	// put it in front of bucket 
	
	if (self->records[hval])
	{ 
		HashRecord_next_(record, self->records[hval]); 
	}
	
	self->records[hval] = record;
	
	// put it in front of enumeration chain
	
	record->nextRecord = self->first;
	
	if (self->first) 
	{ 
		self->first->previousRecord = record; 
	}
	
	self->first = record;
	
	self->count ++;
	
	if (self->count > self->size * HASH_COUNT_TO_SIZE_RATIO)
	{ 
		Hash_rehash(self); 
	}
	
	//Hash_verify(self);
}

void Hash_removeKey_(Hash *self, void *key)
{ 
	unsigned int hval = HASHFUNCTION(key, self->size);
	HashRecord *record = self->records[hval];
	HashRecord *lastRecord = (HashRecord *)NULL;
	
	while (record)
	{
		if (record->key == key) 
		{ 
			HashRecord *previousRecord = record->previousRecord;
			HashRecord *nextRecord = record->nextRecord;
			
			// remove from enumeration chain 
			
			if (previousRecord)
			{ 
				previousRecord->nextRecord = nextRecord; 
			}
			else
			{ 
				self->first = nextRecord; 
			}
			
			if (nextRecord)
			{ 
				nextRecord->previousRecord = previousRecord; 
			}
			
			// remove from bucket chain
			
			if (lastRecord) 
			{ 
				HashRecord_next_(lastRecord, record->next); 
			}
			else
			{ 
				self->records[hval] = record->next; 
			} 
			
			HashRecord_free(record);
			self->count --;
			return;
		}
		lastRecord = record;
		record = record->next;
	}
}

void *Hash_firstKey(Hash *self)
{
	self->current = self->first;
	return (self->current) ? self->current->key : (void *)NULL;
}

void *Hash_nextKey(Hash *self)
{
	if (self->current)
	{ 
		self->current = self->current->nextRecord; 
		
		if (self->current)
		{ 
			return self->current->key; 
		}
	}
	return (void *)NULL;
}

void *Hash_firstValue(Hash *self)
{
	void *k = Hash_firstKey(self);
	return k ? Hash_at_(self, k) : NULL;
}

void *Hash_nextValue(Hash *self)
{
	void *k = Hash_nextKey(self);
	return k ? Hash_at_(self, k) : NULL;
}

void Hash_verify(Hash *self)
{
	HashRecord *r = self->first;
	size_t c = 0;
	
	while (r)
	{
		c++;
		
		if (!Hash_at_(self, r->key))
		{
			printf("Hash_verify() Hash_at_ failed\n");
			exit(1);
		}
		
		r = r->nextRecord;
	}
	
	if (c != self->count)
	{
		printf("Hash_verify() failed - next chain missing items or count wrong\n");
		exit(1);
	}
}

size_t Hash_count(Hash *self) 
{ 
	return self->count; 
}

void Hash_removeValue_(Hash *self, void *value)
{
	// not efficient
	
	for (;;) 
	{
		int index = Hash_indexForValue_(self, value);
		void *key;
		
		if (index == -1) 
		{
			break; 
		}
		
		key = Hash_recordAt_(self, index)->key;
		Hash_removeKey_(self, key);    
	}
}

HashRecord *Hash_recordAt_(Hash *self, int index)
{
	int i = 0;
	HashRecord *record = self->first;
	
	while (record)
	{
		if (i == index) 
		{
			return record; 
		}
		
		record = record->nextRecord;
		i ++;
	}
	return (HashRecord *)NULL;
}

void *Hash_keyAt_(Hash *self, int index)
{
	HashRecord *record = Hash_recordAt_(self, index);
	
	if (record) 
	{ 
		return record->key; 
	}
	
	return (void *)NULL;
}

void *Hash_valueAt_(Hash *self, int index)
{
	HashRecord *record = Hash_recordAt_(self, index);
	
	if (record) 
	{ 
		return record->value; 
	}
	
	return (void *)NULL;
}

int Hash_indexForValue_(Hash *self, void *v)
{
	int index = 0;
	HashRecord *record = self->first;
	
	while (record)
	{
		if (record->value == v) 
		{
			return index;
		}
		
		record = record->nextRecord;
		index ++;
	}
	
	return -1;
}

// perform -------------------------------------------------- 

void Hash_do_(Hash *self, HashDoCallback *callback)
{
	HashRecord *record = self->first;
	
	while (record)
	{
		(*callback)(record->value);
		record = record->nextRecord;
	}
} 

void Hash_doOnKey_(Hash *self, HashDoCallback *callback)
{
	HashRecord *record = self->first;
	
	while (record)
	{
		(*callback)(record->key);
		record = record->nextRecord;
	}
}

// testing ---------------------------- 

/*
void Hash_UnitTestRemove(void)
{
	Hash *h = Hash_new();
	size_t i, max = 20000;
	
	for (i = 0; i < max; i ++)
	{
		void *k = (void *)(i);
		Hash_at_put_(h, k, k);
	}
	
	if (Hash_count(h) != max)
	{
		printf("wrong count\n");
		exit(-1);
	}
	
	for (i = 0; i < max; i ++)
	{
		void *k = (void *)(i);
		Hash_removeKey_(h, k);
	}
	
	if (Hash_count(h))
	{
		printf("hash not empty!\n");
		exit(-1);
	}
	
	for (i = 0; i < max; i ++)
	{
		void *k = (void *)(i);
		
		if (Hash_at_(h, k))
		{
			printf("hash record not removed!\n");
			exit(-1);
		}
	}
	
	Hash_free(h);    
}

void Hash_UnitTestBasic(void)
{    
	// just add and remove tons of random records to see if we can cause a segfault 
	
	Hash *h = Hash_new();
	int i;
	
	for (i = 0; i < 100000; i ++)
	{
		void *k = (void *)(rand() % 500);
		
		if (i % 3 == 0)
		{
			Hash_removeKey_(h, k);
		}
		else
		{
			Hash_at_put_(h, k, k);
		}
	}
	
	Hash_free(h);    
}

void Hash_UnitTest(void)
{
	Hash_UnitTestBasic();
	Hash_UnitTestRemove();
}
*/

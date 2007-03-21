/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
docDescription("""
PHash - Perfect Hash
keys and values are references (they are not copied or freed)
key pointers are assumed unique

This is a perfect hash. When a new key collides with an
existing one, the storage size is increased by one and the records are
rehashed until no collisions are found. This means lookups are
fast but the number of keys must remain small(<100) - otherwise 
the storage size will get too big.
""")
*/

#ifdef PHASH_C 
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#include <stdio.h>

#define PHASH_RECORDAT_(self, index) (self->records + index)

IOINLINE unsigned int PHash_count(PHash *self) 
{
	return self->numKeys; 
}

/*
 IOINLINE void PHash_recordAt_(PHash *self, unsigned int index)
 { 
	 return (self->records + index); 
 }
 */

// --- sp2 -------------------------------- 

IOINLINE unsigned int maskOfSize(unsigned int size)
{
	unsigned int mask = (~ 0);
	mask = mask >> (32-size);
	return mask;
}

IOINLINE int intLog2(int n)
{
	int i = 0;
	
	if (!n) 
	{
		return 0;
	}
	
	while (n >> i) 
	{ 
		i ++; 
	}
	
	return i;
}

IOINLINE int intPow2(int x)
{
	return 1 << x;
}

IOINLINE void PHash_setTableSize_(PHash *self, unsigned int size)
{
	/*
	 if (self->tableSize > 1024)
	 {
		 printf("PHash_%p tableSize is %i\n", (void *)self, self->tableSize); 
	 }
	 */
	
	if (size < self->tableSize / 8)
	{
		// we don't need all this extra space anymore 
		free(self->records);
		self->records = (PHashRecord *)calloc(1, size * sizeof(PHashRecord));
	}
	else
	{ 
		self->records = (PHashRecord *)realloc(self->records, size * sizeof(PHashRecord)); 
	}
	
	self->tableSize = size;
}

#define PHASH_BITS_PER_POINTER 32

/* 
this derefs the pointer so it points to the data pointer of the object, which for
 Io's Strings is their ByteArray C object (Io Strings are unique) 
 
#define PHASHFUNCTION(v, self) (int)((ptrdiff_t)*((void **)v))%(self->size - 1)
 
 need to change this to make sure it works on 64 bit machines 
 */

IOINLINE void *PHashKey_value(void *key) 
{
	return key;
	//return (*((void **)key));
     //return (ptrdiff_t)keyP;
}

#define PHASH_USE_DEREF_KEY

#ifdef PHASH_USE_DEREF_KEY
IOINLINE unsigned int PHashKey_isEqual_(void *key1, void *key2) 
{
	return key2 && (PHashKey_value(key1) == PHashKey_value(key2));        
}
#else
IOINLINE unsigned int PHashKey_isEqual_(void *key1, void *key2) 
{
	return key1 == key2;        
}
#endif

IOINLINE unsigned int PHash_hash(PHash *self, void *key)
{  
	ptrdiff_t k = (ptrdiff_t)PHashKey_value(key);
	return ((k >> self->a) ^ (k >> self->b)) & self->mask;
}

IOINLINE void PHash_prepareRehash(PHash *self)
{
    //unsigned int size = intPow2(intLog2(self->numKeys -1));
	unsigned int size = 1;
	
	while (size < self->numKeys) 
	{
		size = size << 1;
	}
	
	self->a = 1;
	self->b = 0;
	
	if (size < self->numKeys)
	{
		printf("size < numKeys\n");
		exit(1);
	}
    //printf("numKeys: %i tableSize: %i\n", self->numKeys, size);
	
	PHash_setTableSize_(self, size); 
	self->mask = maskOfSize(intLog2(self->tableSize)-1);
}

IOINLINE void PHash_nextRehash(PHash *self)
{
	/*
	 self->c ++;
	 if (self->c >= self->b)
	 */
	
	self->b ++;
	
	if (self->b >= self->a)
	{ 
		self->b = 0;
		self->a ++; 
		
		if (self->a == 32) 
		{
			PHash_setTableSize_(self, self->tableSize * 2); 
			self->mask = maskOfSize(intLog2(self->tableSize) - 1);
			self->a = 0;
		}
	}
	
	if (self->tableSize > 20000)
	{
		printf("ERROR: PHash %p tablesize: %i to big, %i keys\n", 
			  (void *)self, self->tableSize, self->numKeys);
		exit(1);
	}
}

// ----------------------------------- 

IOINLINE void PHash_clean(PHash *self)
{ 
	/*
	 if (self->size > 50)
	 { 
		 newRecords = (PHashRecord *)realloc(newRecords, sizeof(PHashRecord)*8); 
	 }
	 */
	memset(self->records, 0, sizeof(PHashRecord) * self->tableSize); 
	self->numKeys = 0;
}

IOINLINE void *PHash_at_(PHash *self, void *key)
{	
#ifdef PHASH_USE_CACHE
	{
		PHashRecord *record = PHASH_RECORDAT_(self, self->cachedLookup); 
		if (PHashKey_isEqual_(key, record->key)) return record->value;
	}
#endif
	
	{
	unsigned int index = PHash_hash(self, key);
		
#ifdef PHASH_USE_CACHE
	self->cachedLookup = index;
#endif

	PHashRecord *record = PHASH_RECORDAT_(self, index);
	return PHashKey_isEqual_(key, record->key) ? record->value : (void *)NULL;
	}
}

IOINLINE unsigned char PHash_at_update_(PHash *self, void *key, void *value)
{
	PHashRecord *record = PHASH_RECORDAT_(self, PHash_hash(self, key));
	
	if (record->key)
	{
		// already a matching key, replace it 
		if (PHashKey_isEqual_(key, record->key))
		{
			if (record->value == value) 
			{
				return 0; // return 0 if no change 
			}
			
			record->value = value;
			return 1;
		}
	}
	
	return 0;
}

IOINLINE void PHash_at_put_(PHash *self, void *key, void *value)
{
	PHashRecord *record = PHASH_RECORDAT_(self, PHash_hash(self, key));
	
	if (NULL == record->key)
	{
		record->key   = key;
		record->value = value;
		self->numKeys ++;
	}
	else
	{
		if (PHashKey_isEqual_(key, record->key)) // if keys match, replace value 
		{
			/*
			 if (record->value == value) // return 0 if no change 
			 {
				 return 0;
			 }
			 */
			
			record->value = value; 
			return; // return 1 if the table was modified 
		}
		else // it's a collision, rehash with new record 
		{ 
			// collapse the old records into a list 
			PHash_collapseRecordsAndAddKey_value_(self, key, value);
			PHash_rehashWithCollapsedRecords(self);
		}
	}
	
	return;
}

IOINLINE void PHash_removeKey_(PHash *self, void *key)
{
	unsigned int h = PHash_hash(self, key);
	PHashRecord *record = PHASH_RECORDAT_(self, h);
	void *rkey = record->key;
	
	if (rkey && PHashKey_value(rkey) == PHashKey_value(key))
	{
		self->numKeys --;
		memset(record, 0, sizeof(PHashRecord));
	}
}

IOINLINE void PHash_doOnKeyAndValue_(PHash *self, PHashDoCallback *callback)
{
	PHashRecord *record;
	unsigned int i, size = self->tableSize;
	
	for (i = 0; i < size; i ++)
	{
		record = PHASH_RECORDAT_(self, i);
		
		if (record->key) 
		{ 
			(*callback)(record->key); 
			(*callback)(record->value); 
		}
	}
}

#undef IO_IN_C_FILE
#endif

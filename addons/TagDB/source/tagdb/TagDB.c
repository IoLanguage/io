/*#io
QDBM ioDoc(
		 docCopyright("Steve Dekorte", 2002)
		 docLicense("BSD revised")
		 docDescription("A key/value idsbase.")
		 docCategory("Databases")
		 */

#include "TagDB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TagDB *TagDB_new(void)
{
	TagDB *self = calloc(1, sizeof(TagDB));
	self->symbols = SymbolDB_new();
	self->results = KeyIdArray_new();
	return self;
}

void TagDB_free(TagDB *self) 
{	
	TagDB_close(self);
	SymbolDB_close(self->symbols);
	SymbolDB_free(self->symbols);
	KeyIdArray_free(self->results);
	if (self->path) free(self->path);
	if (self->k2tPath) free(self->k2tPath);
	free(self);
}

void TagDB_delete(TagDB *self)
{
	remove(self->k2tPath);
	SymbolDB_delete(self->symbols);
}

void TagDB_setPath_(TagDB *self, char *path)
{
	char *ext = ".k2t";
	self->path = strcpy(realloc(self->path, strlen(path) + 1), path);
	self->k2tPath = realloc(self->k2tPath, strlen(self->path) + strlen(ext) + 1);
	strcat(strcpy(self->k2tPath, path), ext);
	
	SymbolDB_setPath_(self->symbols, path);
}

char *TagDB_path(TagDB *self)
{
	return self->path;
}

// -------------------------------------------------------- 

#include <assert.h>

int TagDB_read(TagDB *self)
{
	VILLA *v = self->keyToTags;
	
	keyid_t key;
	int keySize;
	
	tagid_t *value;
	int valueSize;
	
	size_t index = 0;
	size_t size = vlrnum(v);
	
	self->items = realloc(self->items, size * sizeof(void *));
	
	vlcurfirst(v);
	
	for(;;)
	{
		char *keyValue = vlcurkey(v, &keySize);
		
		if (!keyValue) break;
		if (keySize != sizeof(keyid_t)) 
		{
			printf("TagDB error: keySize != sizeof(keyid_t)\n");
			return 0;
		}
		
		key = ((keyid_t *)keyValue)[0];
		value = (tagid_t *)vlcurval(v, &valueSize);
		
		if(valueSize % sizeof(tagid_t) != 0)
		{
			printf("TagDB error: valueSize %% sizeof(tagid_t) != 0\n");
			return 0;
		}
		
		self->items[index] = TagDBItem_new(key, value, valueSize / sizeof(tagid_t));
		
		if (keyValue) free(keyValue);
		if (value) free(value);
		index ++;
		vlcurnext(v);
	}
	
	self->itemCount = index;
	return 1;
}

int TagDB_open(TagDB *self)
{
	TagDB_close(self);
	
	self->keyToTags = vlopen_andRepairIfNeeded(self->k2tPath, VL_OWRITER | VL_OCREAT, VL_CMPUINT);
	
	if (!self->keyToTags) return 0;
		
	SymbolDB_setPath_(self->symbols, self->path);
	return TagDB_read(self) && 	SymbolDB_open(self->symbols);
}

int TagDB_close(TagDB *self)
{
	if (self->keyToTags) 
	{
		vlclose(self->keyToTags);
		self->keyToTags = NULL;
	}
	
	if (self->items)
	{
		size_t i;
		
		for (i = 0; i < self->itemCount; i ++)
		{
			TagDBItem_free(self->items[i]);
		}
		
		free(self->items);
		self->items = NULL;
		
		self->itemCount = 0;
	}
	
	return 1;
}

size_t TagDB_indexToInsertKey(TagDB *self, keyid_t key)
{ 
	long low  = -1; 
	long high = self->itemCount;
	
	// most will be appends
	
	if (self->itemCount > 0 && key > self->items[self->itemCount - 1]->key) 
	{
		return self->itemCount;
	}
	
	while (high - low > 1)
	{
		long i = (high + low) / 2;
		keyid_t k;
		
		k = self->items[i]->key;
		
		if (k == key) { return i; }
		if (k > key) { high = i; } else { low  = i; }
	}
	
	return high;
}

long TagDB_indexForKey(TagDB *self, keyid_t key)
{	
	long index = TagDB_indexToInsertKey(self, key);
	if (index == self->itemCount) return -1;
	return (self->items[index]->key == key) ? index : -1;
}

int TagDB_atKey_putTags_(TagDB *self, keyid_t key, TagIdArray *tags)
{
	size_t itemCount = self->itemCount;
	long index = TagDB_indexToInsertKey(self, key);
	TagDBItem *item;
	int result;
		
	if (index == itemCount || self->items[index]->key != key)
	{		 
		//printf("insert %i\n", index);
		
		item = TagDBItem_new(key, tags->ids, tags->size);
		self->items = realloc(self->items, (self->itemCount + 1) * sizeof(void *));
		
		if (index != itemCount)
		{
			memmove(self->items + index + 1, self->items + index, 
				   (itemCount - index) * sizeof(void *));
		}
		
		self->items[index] = item;
		self->itemCount ++;
	}
	else
	{
		//printf("update %i\n", index);

		item = self->items[index];
		TagDBItem_setTags_(item, tags);
	}
	
	// insert/update on-disk
		
	vltranbegin(self->keyToTags);
	result = vlput(self->keyToTags, 
				(const char *)&(item->key), sizeof(keyid_t), 
				(const char *)item->tags.ids, item->tags.size * sizeof(tagid_t), VL_DOVER);
	vltrancommit(self->keyToTags);
	
	return result;
}

// access

TagIdArray *TagDB_tagsAt_(TagDB *self, keyid_t key)
{
	long index = TagDB_indexForKey(self, key);
	if (index == -1) return NULL;
	{
		TagDBItem *item = self->items[index];
		return &(item->tags);
	}
}

// remove

int TagDB_removeKey_(TagDB *self, keyid_t key)
{
	int result = 0;	
	long index = TagDB_indexForKey(self, key);
	
	if (index != -1)
	{
		// remove in-memory
		
		memmove(self->items + index, self->items + index + 1, 
			   (self->itemCount - (index + 1)) * sizeof(void *));
		self->itemCount --;
		
		// remove on-disk
		
		vltranbegin(self->keyToTags);
		result = vloutlist(self->keyToTags, (const char *)&key, sizeof(keyid_t));
		vltrancommit(self->keyToTags);
	}
	
	return result;
}

size_t TagDB_size(TagDB *self)
{
	if (!self->keyToTags) return 0;
	return self->itemCount;
}

#ifdef WIN32
#define inline __inline
#endif
inline static int TagDBItem_matches_(TagDBItem *self, TagIdArray *tags)
{
	tagid_t *a = tags->ids;
	int aSize  = tags->size;
	
	tagid_t *b = self->tags.ids;
	int bSize = self->tags.size;
	
	int i = 0, j = 0;
	
	// return true if all of a are found in b - assume no duplicate items
	
	//if (aSize > bSize) return 0;
	
	while (i < aSize)
	{		
		for(; j < bSize; j ++)
		{
			if (a[i] <  b[j]) { return 0;  } // since they are sorted
			if (a[i] == b[j]) { goto next; }			
		}
		
		return 0;
next:
		i ++;
	}
	
	return 1;
}

KeyIdArray *TagDB_keysForTags_(TagDB *self, TagIdArray *tags)
{	
	size_t size = self->itemCount;
	size_t index; 
	TagDBItem **items = self->items;
	KeyIdArray *results = self->results;
	
	TagIdArray_sort(tags);
	
	results->ids = realloc(results->ids, size * sizeof(keyid_t));
	results->size = 0;
	
	for (index = 0; index < size; index ++)
	{
		TagDBItem *item = *items;
				
		if (TagDBItem_matches_(item, tags))
		{
			results->ids[results->size] = item->key;
			results->size ++;
		}
		
		items ++;
	}
	
	return results;
}

void TagDB_show(TagDB *self)
{
	size_t i;
	
	printf("TagDB:\n");
	
	for (i = 0; i < self->itemCount; i ++)
	{
		TagDBItem *item = self->items[i];
		TagDBItem_show(item);
	}
	
	printf("\n");
}

Datum *TagDB_symbolForId_(TagDB *self, symbolid_t key)
{
	return SymbolDB_symbolForId_(self->symbols, key);
}

symbolid_t TagDB_idForSymbol_size_(TagDB *self, const char *s, size_t size)
{
	symbolid_t id;
	Datum d;
	d.data = (unsigned char *)s;
	d.size = size;
	id = SymbolDB_idForSymbol_(self->symbols, &d);
	//printf("%s %i\n", s, (int)id);
	return id;
}

symbolid_t TagDB_idForSymbol_(TagDB *self, const char *s)
{
	return TagDB_idForSymbol_size_(self, s, strlen(s));
}

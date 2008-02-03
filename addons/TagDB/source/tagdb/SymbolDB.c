
#include "SymbolDB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

SymbolDB *SymbolDB_new(void)
{
	SymbolDB *self = calloc(1, sizeof(SymbolDB));
	return self;
}

void SymbolDB_free(SymbolDB *self) 
{	
	if (self->path) free(self->path);
	if (self->s2iPath) free(self->s2iPath);
	if (self->i2sPath) free(self->i2sPath);
	SymbolDB_close(self);
	free(self);
}

void SymbolDB_setPath_(SymbolDB *self, char *path)
{
	char *ext;
	self->path = strcpy(realloc(self->path, strlen(path) + 1), path);
	
	ext = ".s2i";
	self->s2iPath = realloc(self->s2iPath, strlen(self->path) + strlen(ext) + 1);
	strcat(strcpy(self->s2iPath, path), ext);
	
	ext = ".i2s";
	self->i2sPath = realloc(self->i2sPath, strlen(self->path) + strlen(ext) + 1);
	strcat(strcpy(self->i2sPath, path), ext);	
}

char *SymbolDB_path(SymbolDB *self)
{
	return self->path;
}

void SymbolDB_delete(SymbolDB *self)
{
	remove(self->s2iPath);
	remove(self->i2sPath);
}

// -------------------------------------------------------- 

int SymbolDB_open(SymbolDB *self)
{
	SymbolDB_close(self);
	
	self->s2i = vlopen_andRepairIfNeeded(self->s2iPath, VL_OWRITER | VL_OCREAT, VL_CMPLEX);
	self->i2s = vlopen_andRepairIfNeeded(self->i2sPath, VL_OWRITER | VL_OCREAT, VL_CMPUINT);
	
	if (self->s2i && self->i2s) 
	{
		return 1;	
	}
	
	SymbolDB_close(self);
	return 0;
}

int SymbolDB_close(SymbolDB *self)
{
	if (self->s2i) 
	{
		vlclose(self->s2i);		
		self->s2i = NULL;
	}
	
	if (self->i2s) 
	{
		vlclose(self->i2s);
		self->i2s = NULL;
	}
	
	return 1;
}

size_t SymbolDB_size(SymbolDB *self)
{
	return self->s2i ? vllnum(self->s2i) : 0;
}


void SymbolDB_show(SymbolDB *self)
{
	
	printf("SymbolDB:\n");
	
	
	printf("\n");
}

// high level API

Datum *SymbolDB_symbolForId_(SymbolDB *self, symbolid_t key)
{
	int size;
	unsigned char *name = (unsigned char *)vlget(self->i2s, (const char *)&key, sizeof(symbolid_t), &size);	
	return name == NULL ? NULL : Datum_newData_size_copy_(name, size, 0); 
}

symbolid_t SymbolDB_idForSymbol_(SymbolDB *self, Datum *symbol)
{
	int idSize;
	symbolid_t id;
	char *data = vlget(self->s2i, (const char *)Datum_data(symbol), Datum_size(symbol), &idSize);
	
	if (data) // if the symbol is known, return it's id
	{
		// should probably verify that's it's in the id2symbol table and insert it if not
		id = *(symbolid_t *)data;
	}
	else // otherwise, create an available id
	{		
		vlcurlast(self->i2s);

		data = vlcurkey(self->i2s, &idSize);
		id = (!data) ? 0 : (*(symbolid_t *)data) + 1;
		
		//assert(keySize == sizeof(symbolid_t));
		
		// add a symbol entry
		
		vltranbegin(self->i2s);
		vlput(self->i2s, 
					(const char *)&(id), sizeof(symbolid_t), 
					(const char *)Datum_data(symbol), Datum_size(symbol), 
					VL_DOVER);
		vltrancommit(self->i2s);	
		
		vltranbegin(self->s2i);
		vlput(self->s2i, 
					(const char *)Datum_data(symbol), Datum_size(symbol), 
					(const char *)&(id), sizeof(symbolid_t), 
					VL_DOVER);
		vltrancommit(self->s2i);	
	}
	
	return id;
}

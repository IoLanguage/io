#include "TagDB.h"
#include <stdio.h>

int main(int argc, const char * argv[]) 
{
	// instantiate a database
	
	TagDB *tdb = TagDB_new();

	// open it
		
	TagDB_setPath_(tdb, "testdb");
	
	if(TagDB_open(tdb) == 0)
	{
		exit(-1);
	}
	
	// insert some keys and their associated tags
	// updates are done the same way
	
	{
		TagIdArray *tags = TagIdArray_new();
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "black"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "1991"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "nsx"));
		TagDB_atKey_putTags_(tdb, TagDB_idForSymbol_(tdb, "joe"), tags);
		TagIdArray_free(tags);
	}
	
	{
		TagIdArray *tags = TagIdArray_new();
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "1991"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "esprit"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "black"));
		TagDB_atKey_putTags_(tdb, TagDB_idForSymbol_(tdb, "bob"), tags);
		TagIdArray_free(tags);
	}	
	
	{
		TagIdArray *tags = TagIdArray_new();
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "1991"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "ferrari"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "red"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "355"));
		TagDB_atKey_putTags_(tdb, TagDB_idForSymbol_(tdb, "alice"), tags);
		TagIdArray_free(tags);
	}	
		
	// perform a search
	
	{
		size_t i;
		TagIdArray *tags = TagIdArray_new();
		KeyIdArray *results;
		
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "red"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "1991"));
		
		results = TagDB_keysForTags_(tdb, tags); 
		
		// we *don't* free the results - make a copy if you need to keep them

		printf("%i matches\n", (int)KeyIdArray_size(results));
		
		for(i = 0; i < KeyIdArray_size(results); i ++)
		{
			Datum *d = TagDB_symbolForId_(tdb, KeyIdArray_at_(results, i));
			printf("%i: %s\n", (int)i, (char *)Datum_data(d));
			Datum_free(d);
		}
		
		TagIdArray_free(tags);

	}
	
	// remove a key

	TagDB_removeKey_(tdb, 11);

     // close and delete it
	
	TagDB_close(tdb);
	TagDB_delete(tdb);
	
	return 0;
}

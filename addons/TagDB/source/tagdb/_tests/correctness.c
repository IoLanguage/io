#include <stdio.h>
#include <time.h>
#include <string.h>

#include "TagDB.h"

static TagDB *tdb;

void test(char *message, int ok)
{	
	printf("  %s: %s\n", message, ok ? "OK" : "FAILED");
	if (!ok) { exit(-1); }
}

void doSomeInserts(void)
{
	{
		TagIdArray *tags = TagIdArray_new();
		
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "black"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "1991"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "nsx"));
		
		test("TagDB_atKey_putTags_", TagDB_atKey_putTags_(tdb, TagDB_idForSymbol_(tdb, "joe"), tags));
		
		TagIdArray_free(tags);		
		test("insert1", TagDB_size(tdb) == 1);
	}
		
	{
		TagIdArray *tags = TagIdArray_new();
		
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "1996"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "lotus"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "esprit"));
		
		test("TagDB_atKey_putTags_", TagDB_atKey_putTags_(tdb, TagDB_idForSymbol_(tdb, "bob"), tags));
		
		TagIdArray_free(tags);		
		test("insert2", TagDB_size(tdb) == 2);
	}
	
	{
		TagIdArray *tags = TagIdArray_new();
		
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "red"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "nsx"));
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "1991"));
		
		test("TagDB_atKey_putTags_", TagDB_atKey_putTags_(tdb, TagDB_idForSymbol_(tdb, "alice"), tags));
		
		TagIdArray_free(tags);		
		test("insert3", TagDB_size(tdb) == 3);
	}
}

void verifyInserts(void)
{
	Datum *s;

	{
		TagIdArray *tags = TagDB_tagsAt_(tdb, TagDB_idForSymbol_(tdb, "joe"));
		test("222 tags", (int)tags);
		
		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 0));
		test("verify key joe tag 0", !strcmp((char *)s->data, "black"));
		Datum_free(s);

		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 1));
		test("verify key joe tag 0", !strcmp((char *)s->data, "1991"));
		Datum_free(s);

		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 2));
		test("verify key joe tag 0", !strcmp((char *)s->data, "nsx"));
		Datum_free(s);
	}
	
	{
		TagIdArray *tags = TagDB_tagsAt_(tdb, TagDB_idForSymbol_(tdb, "bob"));
		test("111 tags", (int)tags);

		
		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 0));
		test("verify key bob tag 0", !strcmp((char *)s->data, "1996"));
		Datum_free(s);

		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 1));
		test("verify key bob tag 1", !strcmp((char *)s->data, "lotus"));
		Datum_free(s);

		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 2));
		test("verify key bob tag 2", !strcmp((char *)s->data, "esprit"));
		Datum_free(s);
	}

	
	{
		TagIdArray *tags = TagDB_tagsAt_(tdb, TagDB_idForSymbol_(tdb, "alice"));
		test("333 tags", (int)tags);
		
		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 0));
		test("verify key alice tag 0", !strcmp((char *)s->data, "1991"));
		Datum_free(s);

		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 1));
		test("verify key alice tag 1", !strcmp((char *)s->data, "nsx"));
		Datum_free(s);

		s = TagDB_symbolForId_(tdb, TagIdArray_at_(tags, 2));
		test("verify key alice tag 2", !strcmp((char *)s->data, "red"));
		Datum_free(s);
	}
}

int main (int argc, const char * argv[]) 
{	
	tdb = TagDB_new();

	printf("\ntdb correctness tests:\n");
	
	TagDB_setPath_(tdb, "testdb");
	TagDB_delete(tdb);
	test("open", TagDB_open(tdb));

	doSomeInserts();
	verifyInserts();
	
	// do some searches
	
	{		
		TagIdArray *tags = TagIdArray_new();
		KeyIdArray *keys;
		
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "nsx"));
		keys = TagDB_keysForTags_(tdb, tags);
		test("search nsx", KeyIdArray_size(keys) == 2);
		
		TagIdArray_append_(tags, TagDB_idForSymbol_(tdb, "black"));
		keys = TagDB_keysForTags_(tdb, tags);
		test("search nsx, black", KeyIdArray_size(keys) == 1);
		
		TagIdArray_free(tags);
	}

	test("close", TagDB_close(tdb));
	
	printf("  reopening...\n");
	
	test("open", TagDB_open(tdb));
	
	verifyInserts();	
	
	// remove keys
	
	{
		TagDB_removeKey_(tdb, TagDB_idForSymbol_(tdb, "bob"));
		test("remove bob", TagDB_size(tdb) == 2);
		TagDB_removeKey_(tdb, TagDB_idForSymbol_(tdb, "joe"));
		test("remove joe", TagDB_size(tdb) == 1);		
		TagDB_removeKey_(tdb, TagDB_idForSymbol_(tdb, "alice"));
		test("remove alice", TagDB_size(tdb) == 0);	
	}
		
	test("close", TagDB_close(tdb));	
	TagDB_delete(tdb);
	
	printf("  ALL TESTS PASSED\n\n"); 
	return 0;
}

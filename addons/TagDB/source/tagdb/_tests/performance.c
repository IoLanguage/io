#include <stdio.h>
#include <time.h>

#include "TagDB.h"

static clock_t timerStart;

void Timer_Begin(void)
{	
	timerStart = clock();
}

double Timer_End(void)
{
	return ((double)clock() - (double)timerStart) / (double)CLOCKS_PER_SEC;
}

void Timer_EndShowNumPerSecond(int num, char *op)
{
	double e = Timer_End();
	
	if (e)
	{
		char *suffix = "";
		int v = num/e;
		if (v > 10000000) { v /= 1000000; suffix = "M"; }
		if (v > 10000)    { v /= 1000;    suffix = "K"; }
		
		printf("  %i%s %ss per second\n", v, suffix, op);
	}
}

void Timer_EndShowMillisecondsPer(int num, char *op)
{
	if (num) printf("  %ims per %s\n", (int)(Timer_End()*1000/num), op);
}

int main (int argc, const char * argv[]) 
{
	TagDB *tagDB = TagDB_new();
	int i;
	int max = 10000;
	
	printf("\ntagdb performance tests:\n");
	
	Timer_Begin();
	
	remove("testdb");
	
	TagDB_setPath_(tagDB, "testdb");
	
	if(TagDB_open(tagDB) == 0)
	{
		exit(-1);
	}
		
	Timer_EndShowNumPerSecond(TagDB_size(tagDB), " load-record-from-disk op");
	
	printf("  inserting...\n");
	
	if (TagDB_size(tagDB) < max)
	{		
		Timer_Begin();
		
		for (i = TagDB_size(tagDB); i < max; i = i + 1)
		{
			TagIdArray *tags = TagIdArray_new();
			tagid_t tid = rand() % 3;
			
			//printf("inserting %i\n", i);
			TagIdArray_append_(tags, tid);
			tid += 1 + rand() % 3;
			TagIdArray_append_(tags, tid);
			tid += 1 + rand() % 3;
			TagIdArray_append_(tags, tid);
			
			TagDB_atKey_putTags_(tagDB, i, tags);
			
			TagIdArray_free(tags);
		}
		
		Timer_EndShowNumPerSecond(max, "insert");
		
	}
	
	printf("  searching...\n");
	
	{
		int n, numSearches = 10000;
		
		TagIdArray *tags = TagIdArray_new();
		KeyIdArray *results;
		
		TagIdArray_append_(tags, rand() % 5);
		TagIdArray_append_(tags, 3 + rand() % 3);
	
		Timer_Begin();
		for(n = 0; n < numSearches; n ++)
		{
			results = TagDB_keysForTags_(tagDB, tags);
		}
		
		//printf("%i matches found\n", (int)results->size);
		
		TagIdArray_free(tags);

		Timer_EndShowNumPerSecond((numSearches * TagDB_size(tagDB)), "records searche");
	}

	printf("  closing...\n");

	TagDB_close(tagDB);
	printf("  reopening...\n");
	
	Timer_Begin();
	TagDB_open(tagDB);
	Timer_EndShowNumPerSecond(TagDB_size(tagDB), "records read");
	
	printf("  removing...\n");
	
	{
		size_t numToRemove = TagDB_size(tagDB);
		
		Timer_Begin();
		
		for (i = 0; i < numToRemove; i = i + 1)
		{
			TagDB_removeKey_(tagDB, i);
		}
		
		Timer_EndShowNumPerSecond(numToRemove, "remove");
	}
	
	
	TagDB_close(tagDB);
	TagDB_delete(tagDB);

	printf("\n");
	
	return 0;
}

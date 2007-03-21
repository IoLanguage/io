// cc text.c -I/opt/local/include -L/opt/local/lib -lqdbm

#include <depot.h>
#include <cabin.h>
#include <villa.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int compareFunc(const char *aptr, int asiz, const char *bptr, int bsiz)
{
	size_t max = asiz < bsiz ? asiz : bsiz;
	return strncmp(aptr, bptr, max);
}

int main(void)
{
	VILLA *villa;
	int size;
	char *s;
	
	printf("compareFunc(\"a\", \"b\") = %i\n", compareFunc("a", 1, "b", 1));
	printf("compareFunc(\"b\", \"a\") = %i\n", compareFunc("b", 1, "a", 1));
	printf("compareFunc(\"a\", \"a\") = %i\n", compareFunc("a", 1, "a", 1));
	printf("compareFunc(\"b\", \"b\") = %i\n", compareFunc("b", 1, "b", 1));
	
	remove("test.qdbm");
	
	if(!(villa = vlopen("test.qdbm", VL_OWRITER | VL_OCREAT, compareFunc)))
	{
		printf("vlopen failed %s\n", dperrmsg(dpecode));
		return 1;
	}

	if(!vltranbegin(villa))
	{
		printf("vltranbegin failed %s\n", dperrmsg(dpecode));
		return 1;	
	}
		
	if(!vlput(villa, "a", 1, "1", 1, DP_DOVER))
	{
		printf("vlput failed %s\n", dperrmsg(dpecode));
		return 1;
	}
	
	if(!vlput(villa, "b", 1, "2", 1, DP_DOVER))
	{
		printf("vlput failed %s\n", dperrmsg(dpecode));
		return 1;
	}
	
	if(!vltrancommit(villa))
	{
		printf("vltrancommit failed %s\n", dperrmsg(dpecode));
		return 1;	
	}
	
	if(!vlclose(villa))
	{
		printf("vlclose failed %s\n", dperrmsg(dpecode));
		return 1;
	}
	
	if(!(villa = vlopen("test.qdbm", VL_OWRITER | VL_OCREAT, compareFunc)))
	{
		printf("vlopen failed %s\n", dperrmsg(dpecode));
		return 1;
	}
	
	s = vlget(villa, "a", 1, &size);
	printf("a = '%s'\n", s ? s : "NULL");

	s = vlget(villa, "b", 1, &size);
	printf("b = '%s'\n", s ? s : "NULL");

	if(!vlclose(villa))
	{
		printf("vlclose failed %s\n", dperrmsg(dpecode));
		return 1;
	}
		
	return 0;
}

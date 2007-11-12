/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 */

#include "Array_enum.h" 

void Array_do_(Array *self, ArrayDoFunc *callback)
{
    size_t i;
    size_t size = self->size;
        
    for (i = 0; i < size; i ++) 
    { 
	void *item = Array_rawAt_(self, i);
	
	if (item) 
	{
	    (*callback)(item); 
	}
    }
}

void Array_do_with_(Array *self, ArrayDoWithFunc *func, void *arg)
{
    size_t i;
    size_t size = self->size;
        
    for (i = 0; i < size; i ++) 
    { 
	void *item = Array_rawAt_(self, i);
	
	if (item) 
	{
	    (*func)(item, arg); 
	}
    }
}

void Array_target_do_(Array *self, void *target, ArrayTargetDoFunc *func)
{
    size_t i;
    size_t size = self->size;
    
    for (i = 0; i < size; i ++) 
    { 
	void *item = Array_rawAt_(self, i);
	
	if (item)
	{
	    (*func)(target, item); 
	}
    }
}

/*
size_t Array_nextMatchIn_(Array *self, Array *items)
{
    void *match;
    size_t index = Array_target_detect_(items, Array_isBeginningOf_, self, &match);
    
    size_t i;
    
    for (i = 0; i < self->size; i ++)
    {
	Datum d1 = Array_datumAt_(self, i);
	size_t j;
	
	for (j = 0; j < items->size; j ++)
	{
	    Array *item = Array_at_(items, j);
	    Datum d2 = Array_datumAt_(item, j);
	    
	    if (Datum_beginsWith_(d1, d2))
	    {
		return Match_Item_index_(item, i);
	    } 
	}
    }
    return Match_Empty();
}

Array *Array_split_(Array *self, Array *items)
{
    return Array_collectOnAllRemaining_context_(self, Split, items);
    
    Array *results = 
    size_t last = 0;
    Match match;
    
    for (;;)
    {
    
	match = Array_matchAnyItem_(self, items);
	
	if (Match_isEmpty(&match)) 
	{
	    break;
	}
	Array_newFrom_to_(last, match.index);
	
	last = Match.index; 
    } 
    
}
*/




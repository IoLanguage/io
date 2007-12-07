/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#include "Base.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MainArgs.h"
#include "PortableStrlcpy.h"

MainArgs *MainArgs_new(void)
{
	MainArgs *self = (MainArgs *)calloc(1, sizeof(MainArgs));
	return self;
}

void MainArgs_free(MainArgs *self)
{
	if (self->argv)
	{
		int i;
		
		for (i = 0; i < self->argc; i ++) 
		{
			free((char *)(self->argv[i]));
		}
		
		free((void*)(self->argv));
	}
	
	free(self);
}

void MainArgs_argc_argv_(MainArgs *self, int argc, const char **argv)
{
	int i;
	
	self->argc = argc;
	// copy by reference since args should be retained on 
	// the C stack until the program exits
	
	self->argv = (const char **)malloc(sizeof(char *)*argc);
	
	for (i = 0; i < argc; i ++)
	{
		const char *s = argv[i];
		size_t len = strlen(s);
		char *ptr = (char *)malloc(len + 1);
		PortableStrlcpy(ptr, s, len + 1);
		self->argv[i] = ptr;
		
		//self->argv[i] = strcpy((char *)malloc(strlen(s)+1), s);
		/*printf("argv[%i] = '%s'\n", i, s);*/
	}
}


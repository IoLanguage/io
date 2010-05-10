#include <stdlib.h>
#include <stdio.h>
#include "PythonData.h"

PythonData *PythonData_new(void)
{
	PythonData *self = calloc(1, sizeof(PythonData));
	self->data = NULL;
	return self;
}

void bug(char *s) {
	printf("%s", s);
	fflush(stdout);
}

void PythonData_free(PythonData *self) {
	if(self->data == (void *) 1) { // Magic / hack value
		Py_Finalize();
	} else if(self->data != NULL) {
		Py_XDECREF(self->data);
	}
	free(self);
}

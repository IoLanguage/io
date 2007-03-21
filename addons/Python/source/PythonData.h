#include <Python.h>
//#include <python2.4/Python.h>

typedef struct
{
	PyObject *data;
} PythonData;

PythonData *PythonData_new(void);
void PythonData_free(PythonData *self);

#ifndef PYTHONDATA_H
#define PYTHONDATA_H

#include <Python.h>

typedef struct
{
	PyObject *data;
} PythonData;

PythonData *PythonData_new(void);
void PythonData_free(PythonData *self);

#endif /* PYTHONDATA_H */

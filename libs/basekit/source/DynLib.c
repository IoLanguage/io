// metadoc DynLib license BSD revised
/*metadoc DynLib history
        based on DllHandle.c, contributed by Daniel A. Koepke
        Reorg, Steve Dekorte, 2003-08-30
        Window fix, Chuck Adams, 2004-02-06
*/

#include "Base.h"
#include "DynLib.h"
#include "PortableStrlcpy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* WASM: no dynamic library loading */
#define RTLD_NOW 0
#define RTLD_GLOBAL 0
static void *dlopen(const char *path, int mode) { return NULL; }
static int dlclose(void *handle) { return 0; }
static const char *dlerror(void) { return "dynamic loading not supported on WASM"; }
static void *dlsym(void *handle, const char *symbol) { return NULL; }

DynLib *DynLib_new(void) {
    DynLib *self = (DynLib *)io_calloc(1, sizeof(DynLib));
    DynLib_setPath_(self, "");
    return self;
}

void DynLib_free(DynLib *self) {
    // DynLib_close(self);
    if (self->path)
        io_free(self->path);
    if (self->initFuncName)
        io_free(self->initFuncName);
    if (self->freeFuncName)
        io_free(self->freeFuncName);
    if (self->error)
        io_free(self->error);
    io_free(self);
}

void DynLib_setPath_(DynLib *self, const char *path) {
    size_t len = strlen(path);
    char *ptr = (char *)io_realloc(self->path, len + 1);
    PortableStrlcpy(ptr, path, len + 1);
    self->path = ptr;
}

char *DynLib_path(DynLib *self) { return self->path; }

void DynLib_setInitFuncName_(DynLib *self, const char *name) {
    size_t len = strlen(name);
    char *ptr = (char *)io_realloc(self->initFuncName, len + 1);
    PortableStrlcpy(ptr, name, len + 1);
    self->initFuncName = ptr;
}

char *DynLib_initFuncName(DynLib *self) { return self->initFuncName; }

void DynLib_setInitArg_(DynLib *self, void *arg) { self->initArg = arg; }

void DynLib_setFreeFuncName_(DynLib *self, const char *name) {
    size_t len = strlen(name);
    char *ptr = (char *)io_realloc(self->freeFuncName, len + 1);
    PortableStrlcpy(ptr, name, len + 1);
    self->freeFuncName = ptr;
}

char *DynLib_freeFuncName(DynLib *self) { return self->freeFuncName; }

void DynLib_setFreeArg_(DynLib *self, void *arg) { self->freeArg = arg; }

void DynLib_setError_(DynLib *self, const char *error) {
    if (error) {
        self->error =
            strcpy((char *)io_realloc(self->error, strlen(error) + 1), error);
    } else {
        if (self->error)
            io_free(self->error);
        self->error = NULL;
    }
}

char *DynLib_error(DynLib *self) { return self->error; }

void DynLib_updateError(DynLib *self) { DynLib_setError_(self, dlerror()); }

unsigned char DynLib_hasError(DynLib *self) { return self->error != NULL; }

void DynLib_open(DynLib *self) {
    self->handle = dlopen(self->path, RTLD_NOW | RTLD_GLOBAL); /* RTLD_LAZY); */
    // self->handle = dlopen(self->path, RTLD_NOW | RTLD_LAZY);
    DynLib_updateError(self);

    if (DynLib_hasError(self)) {
        return;
    }

    if (self->initFuncName) {
        void *f = DynLib_pointerForSymbolName_(self, self->initFuncName);

        if (!f) {
            DynLib_setError_(self, "init function not found");
            return;
        }

        if (self->initArg) {
            // printf("DynLib: opening with 1 arg %p\n", self->initArg);
            (*(DynLibOneArgFunction *)f)(self->initArg);
        } else {
            (*(DynLibNoArgFunction *)f)();
        }
    }
}

unsigned char DynLib_isOpen(DynLib *self) { return self->handle != NULL; }

void DynLib_close(DynLib *self) {
    if (self->freeFuncName) {
        void *f = DynLib_pointerForSymbolName_(self, self->freeFuncName);

        if (!f) {
            DynLib_setError_(self, "io_free function not found");
            return;
        }

        if (self->freeArg) {
            (*(DynLibOneArgFunction *)f)(self->freeArg);
        } else {
            (*(DynLibNoArgFunction *)f)();
        }
    }

    if (self->handle) {
        dlclose(self->handle);
    }

    self->handle = NULL;
}

void *DynLib_pointerForSymbolName_(DynLib *self, const char *symbolName) {
    DynLib_setError_(self, dlerror());
    return dlsym(self->handle, symbolName);
}

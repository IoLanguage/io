#ifndef IOBASE_DEFINED
#define IOBASE_DEFINED 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#if !defined(__wasi__) && !defined(__EMSCRIPTEN__) && !defined(__wasm__)
#include <setjmp.h>
#endif
#include <stdarg.h>
#include <string.h>

#endif

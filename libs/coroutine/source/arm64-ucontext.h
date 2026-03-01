// arm64-ucontext.h: Apple Silicon (ARM64) macOS context switching using ucontext
#ifndef ARM64_UCONTEXT_H
#define ARM64_UCONTEXT_H

// Required for ucontext on modern macOS
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <ucontext.h>

typedef ucontext_t arm64_ucontext_t;

#endif // ARM64_UCONTEXT_H

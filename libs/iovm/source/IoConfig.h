#define _POSIX_C_SOURCE
#define IO_COMPILER_TYPE gcc

/* WASM/WASI — no C stack, no platform-specific jmp_buf detection needed */
#define IO_OS_TYPE wasi
#define IO_PROCESSOR_TYPE wasm32

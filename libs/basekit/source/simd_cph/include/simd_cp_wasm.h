/*****************************************************

  SIMD_CP intrinsics for WebAssembly (wasm_simd128)

  Requires clang with -msimd128 and the runtime's support
  for the WebAssembly SIMD proposal (Node.js >=16, wasmtime,
  all modern browsers).

***************************************************/

#include <wasm_simd128.h>

/* Register/Variable typedefs */

/* simd_m128 is a multi-use 128-bit variable containing 4 ints or 4 floats */
/* ALL SIMD ops in this header use this variable type */

typedef union {
    v128_t v;   /* the packed SIMD data (vector) */
    float f[4]; /* float access */
    int i[4];   /* int access */
    short s[8]; /* short access */
    char c[16]; /* char access */
} simd_m128;

/* Helper commands */

#define simd_load4Floats(v, a, b, c, d)                                        \
    v.f[0] = a;                                                                \
    v.f[1] = b;                                                                \
    v.f[2] = c;                                                                \
    v.f[3] = d
#define simd_load4Ints(v, a, b, c, d)                                          \
    v.i[0] = a;                                                                \
    v.i[1] = b;                                                                \
    v.i[2] = c;                                                                \
    v.i[3] = d

/* print a simd_m128 variable: l=label to print, a=simd_m128 variable */
#define simd_print4Floats(l, a)                                                \
    printf("%s: %f  %f  %f  %f\n", l, a.f[0], a.f[1], a.f[2], a.f[3])
#define simd_print4Ints(l, a)                                                  \
    printf("%s: %d  %d  %d  %d\n", l, a.i[0], a.i[1], a.i[2], a.i[3])
#define simd_print4UInts(l, a)                                                 \
    printf("%s: %d  %d  %d  %d\n", l, (unsigned int)a.i[0],                    \
           (unsigned int)a.i[1], (unsigned int)a.i[2], (unsigned int)a.i[3])
#define simd_print4Hex(l, a)                                                   \
    printf("%s: 0x%08x  0x%08x  0x%08x  0x%08x\n", l, a.i[0], a.i[1], a.i[2],  \
           a.i[3])

/* Integer SIMD intrinsics */

/* Signed ints (4 packed ints) */

#define simd_4i_add(v1, v2, v0) v0.v = wasm_i32x4_add(v1.v, v2.v)
#define simd_4i_sub(v1, v2, v0) v0.v = wasm_i32x4_sub(v1.v, v2.v)
#define simd_4i_mult(v1, v2, v0) v0.v = wasm_i32x4_mul(v1.v, v2.v)

/* no integer SIMD div in wasm_simd128 */
#define simd_4i_div(v1, v2, v0)                                                \
    v0.i[0] = v1.i[0] / v2.i[0];                                               \
    v0.i[1] = v1.i[1] / v2.i[1];                                               \
    v0.i[2] = v1.i[2] / v2.i[2];                                               \
    v0.i[3] = v1.i[3] / v2.i[3]

/* ints logical/bitwise */

#define simd_4i_band(v1, v2, v0) v0.v = wasm_v128_and(v1.v, v2.v)
#define simd_4i_bor(v1, v2, v0) v0.v = wasm_v128_or(v1.v, v2.v)
#define simd_4i_bxor(v1, v2, v0) v0.v = wasm_v128_xor(v1.v, v2.v)

/* Shift-by-scalar: wasm shift intrinsics take (vec, uint32_t) */
#define simd_4i_shftl(v1, x, v0) v0.v = wasm_i32x4_shl(v1.v, (uint32_t)(x))
#define simd_4i_shftr(v1, x, v0) v0.v = wasm_i32x4_shr(v1.v, (uint32_t)(x))

/* Signed shorts (8 packed shorts) */

#define simd_4s_add(v1, v2, v0) v0.v = wasm_i16x8_add(v1.v, v2.v)
#define simd_4s_sub(v1, v2, v0) v0.v = wasm_i16x8_sub(v1.v, v2.v)
#define simd_4s_shftl(v1, x, v0) v0.v = wasm_i16x8_shl(v1.v, (uint32_t)(x))
#define simd_4s_shftr(v1, x, v0) v0.v = wasm_i16x8_shr(v1.v, (uint32_t)(x))

/* Signed chars (16 packed chars) */

#define simd_4c_add(v1, v2, v0) v0.v = wasm_i8x16_add(v1.v, v2.v)
#define simd_4c_sub(v1, v2, v0) v0.v = wasm_i8x16_sub(v1.v, v2.v)
#define simd_4c_shftl(v1, x, v0) v0.v = wasm_i8x16_shl(v1.v, (uint32_t)(x))
#define simd_4c_shftr(v1, x, v0) v0.v = wasm_i8x16_shr(v1.v, (uint32_t)(x))

/* Floating SIMD intrinsics */

/* math */

#define simd_4f_add(v1, v2, v0) v0.v = wasm_f32x4_add(v1.v, v2.v)
#define simd_4f_sub(v1, v2, v0) v0.v = wasm_f32x4_sub(v1.v, v2.v)
#define simd_4f_mult(v1, v2, v0) v0.v = wasm_f32x4_mul(v1.v, v2.v)
#define simd_4f_div(v1, v2, v0) v0.v = wasm_f32x4_div(v1.v, v2.v)
#define simd_4f_sqrt(v1, v0) v0.v = wasm_f32x4_sqrt(v1.v)

/* reciprocal: wasm_simd128 has no rcp opcode, synthesize via div */
#define simd_4f_recp(v1, v0)                                                   \
    v0.v = wasm_f32x4_div(wasm_f32x4_splat(1.0f), v1.v)

/* compares */

#define simd_4f_min(v1, v2, v0) v0.v = wasm_f32x4_min(v1.v, v2.v)
#define simd_4f_max(v1, v2, v0) v0.v = wasm_f32x4_max(v1.v, v2.v)

/* compare masks */

#define simd_4f_meq(v1, v2, v0) v0.v = wasm_f32x4_eq(v1.v, v2.v)
#define simd_4f_mneq(v1, v2, v0) v0.v = wasm_f32x4_ne(v1.v, v2.v)
#define simd_4f_mlt(v1, v2, v0) v0.v = wasm_f32x4_lt(v1.v, v2.v)
#define simd_4f_mgt(v1, v2, v0) v0.v = wasm_f32x4_gt(v1.v, v2.v)
#define simd_4f_mle(v1, v2, v0) v0.v = wasm_f32x4_le(v1.v, v2.v)
#define simd_4f_mge(v1, v2, v0) v0.v = wasm_f32x4_ge(v1.v, v2.v)

/* Utility Functions and non-SIMD */

/* WASM has no prefetch instruction; define these out. */
#define simd_ut_prefetchOTU(p) ((void)(p))
#define simd_ut_prefetchL1(p) ((void)(p))
#define simd_ut_prefetchL2(p) ((void)(p))

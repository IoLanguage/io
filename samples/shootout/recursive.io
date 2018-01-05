#!/usr/bin/env io

/* ------------------------------------------------------------------ */
/* The Great Computer Language Shootout                               */
/* http://shootout.alioth.debian.org/                                 */
/*                                                                    */
/* Whilst this program was successfully tested at a load of:          */
/*                                                                    */
/*     n = 3                                                          */
/*                                                                    */
/* it fails at higher loads due to stack overflow. Having consulted   */
/* the relevant documentation at:                                     */
/*                                                                    */
/*     http://www.iolanguage.com/darcs/Io/_docs/guide.html            */
/*                                                                    */
/* there doesn't appear to be a command-line option or environment    */
/* variable that may be set to increase the in-built stack size, nor  */
/* does the use of the shell setting, 'ulimit -s unlimited', appear to*/
/* increase this limit.                                               */
/*                                                                    */
/* Note: Although this program knowingly fails at n > 3 loads, it was */
/* submitted to replace the existing version which fails due to a     */
/* command-line processing error.                                     */
/*                                                                    */
/* Contributed by Anthony Borla                                       */
/* ------------------------------------------------------------------ */

ack := method(x, y,
  if(x == 0, return y + 1)
  if(y == 0, return ack(x - 1, 1))
  return ack(x - 1, ack(x, y - 1))
)

/* ----------- */

fib := method(n,
  if(n <= 2, return 1)
  return fib(n - 1) + fib(n - 2)
)

/* ----------- */

tak := method(x, y, z,
  if(y < x, return tak(tak(x - 1, y, z), tak(y - 1, z, x), tak(z - 1, x, y)))
  return z
)

/* ----------------------------- */

n := System args at(1) asNumber

// Use interpolation for complex string expressions
"Ack(3,#{n}): #{ack(3, n)}" interpolate println
"Fib(#{(27.0 + n) asString(0,1)}): #{fib(27.0 + n) asString(0,1)}" interpolate println

n := n - 1
"Tak(#{n * 3},#{n * 2},#{n}): #{tak(n * 3, n * 2, n * 1)}" interpolate println

// Use concatenation for simpler ones
("Fib(3): " .. fib(3)) println
("Tak(3.0,2.0,1.0): " .. (tak(3.0, 2.0, 1.0) asString(0,1))) println

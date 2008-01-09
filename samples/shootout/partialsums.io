#!/usr/bin/env io

/* ------------------------------------------------------------------ */
/* The Great Computer Language Shootout                               */
/* http://shootout.alioth.debian.org/                                 */
/*                                                                    */
/* Based on D language implementation by Dave Fladebo                 */
/*                                                                    */
/* Contributed by Anthony Borla                                       */
/* ------------------------------------------------------------------ */

n := System args at(1) asNumber

s0 := 0 ; s1 := 0 ; s2 := 0 ; s3 := 0 ; s4 := 0 ; s5 := 0 ; s6 := 0
s7 := 0 ; s8 := 0 ; d2 := 0 ; d3 := 0 ; ds := 0 ; dc := 0 ; alt := 1

for (d, 1, n,

  d2 = d * d ; d3 = d2 * d ; ds = (d sin) ; dc = (d cos)

  s0 = s0 + ((2.0 / 3.0) pow(d - 1))
  s1 = s1 + 1 / (d sqrt)
  s2 = s2 + 1 / (d * (d + 1))
  s3 = s3 + 1 / (d3 * ds * ds)
  s4 = s4 + 1 / (d3 * dc * dc)
  s5 = s5 + 1 / d
  s6 = s6 + 1 / d2
  s7 = s7 + alt / d
  s8 = s8 + alt / (2 * d - 1)

  alt = -alt
)

(s0 asString(0,9) .. "\t(2/3)^k") println
(s1 asString(0,9) .. "\tk^-0.5") println
(s2 asString(0,9) .. "\t1/k(k+1)") println
(s3 asString(0,9) .. "\tFlint Hills") println
(s4 asString(0,9) .. "\tCookson Hills") println
(s5 asString(0,9) .. "\tHarmonic") println
(s6 asString(0,9) .. "\tRiemann Zeta") println
(s7 asString(0,9) .. "\tAlternating Harmonic") println
(s8 asString(0,9) .. "\tGregory") println

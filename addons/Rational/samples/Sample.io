#!/usr/bin/env io

/* Example usages of Rational Numbers */

r1 := Rational with(1, 2) /* 1/2 */
r1 + r1 == Rational with(1, 1) /* true */
Rational with(1, 1) == Rational with(4, 4) /* true */

r2 := Rational with(1, 4)
r1 + r2 == 0.75 asRational

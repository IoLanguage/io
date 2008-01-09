#!/usr/bin/env io

/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org

   contributed by Ian Osgood */

Sequence pad := method(w, s:=self; while (s size < w, s=" ".. s) )
Number clearBit := method(b, &(1 clone shiftLeft(b) bitwiseComplement) )

sieve := Object clone do(
    flags := Sequence clone do(
        isSet := method(i, at(i/8) at(i%8) != 0)
        clear := method(i, atPut(i/8, at(i/8) clearBit(i%8) ) )
    )
    show := method(n,
        flags setSize((n/8) ceil)
        for (i, 0, flags size - 1, flags atPut(i, 255))
        primes := 0
        for (i, 2, n-1,
            if (flags isSet(i),
                primes = primes + 1
                if (i+i < n, for (j, i+i, n-1, i, flags clear(j) ) )
            )
        )
        writeln("Primes up to", n asString pad(9), primes asString pad(9))
    )
)

n := System args at(1) asNumber
sieve show(10000 shiftLeft(n))
sieve show(10000 shiftLeft(n-1))
sieve show(10000 shiftLeft(n-2))

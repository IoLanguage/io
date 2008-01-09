#!/usr/bin/env io

/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org/
   contributed by Ian Osgood */

fannkuch := method(n,
    a := List clone
    for (i,1,n, a append(i))
    r := n
    counts := a clone
    count := maxFlips := 0

    loop (
        // display the first 30 permutations
        if (count < 30, writeln(a join("")); count = count + 1)

        // eliminate bad choices
        if (a first != 1 and a last != n,
            // pour the batter
            p := a clone
            flips := 0
            // start flipping
            while ((j := p first) > 1,
                // reverse 0..j-1
                i := -1
                while ((i=i+1) < (j=j-1), p swapIndices(i,j))
                flips = flips + 1
            )
            if (flips > maxFlips, maxFlips = flips)
        )

        // generate another permutation
        while (r>1, counts atPut(r-1, r); r=r-1)
        loop (
            // -roll(r)
            a atInsert(r, a removeAt(0))

            if (counts atPut(r, counts at(r) - 1) > 0, break)

            if ((r=r+1) == n, return maxFlips)
        )
    )
)

n := System args at(1) asNumber
f := fannkuch(n)
writeln("Pfannkuchen(", n, ") = ", f)

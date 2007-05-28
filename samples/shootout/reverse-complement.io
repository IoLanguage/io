#!/usr/bin/env io

/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org

   Contributed by Ian Osgood */

seq := Sequence clone do(
    complementAt := method(i,
        "TVGHefCDijMlKNopqYSAABWxRz" at(at(i)-65)  # "A" at(0)
    )
    revComp := method( if (size == 0, return self)
        j := uppercase size
        for (i, 0, size / 2 - 0.25,
            j = j - 1
            t := complementAt(j)
            atPut(j, complementAt(i)) atPut(i, t)
        )
        self
    )
    output := method( if (size == 0, return self)
        width := 60
        start := 0; end := width
        while (end < size,
            slice(start,end) println
            start = end; end = end + width
        )
        slice(start) println
        self
    )
)

input := File standardInput
while (line := input readLine,
    if (line beginsWithSeq(">"),
        seq revComp output empty; line println,
        seq appendSeq(line) )
)
seq revComp output

#!/usr/bin/env io

/* The Computer Language Shootout
   http://shootout.alioth.debian.org

   Contributed by Ian Osgood, Steve Dekorte */


Vector := Sequence clone setItemType("float32") setEncoding("number")

MandelbrotSet := Object clone do(
    max_iterations := 50
    limit          := 2 squared
    dim            := 200
    org_r := -1.5
    org_i := -1
    ext_r :=  2
    ext_i :=  2

    calc := method(
        cr := Vector clone setSize(dim squared)
        ci := Vector clone setSize(dim squared)

        size := 0
        for(y, 0, dim-1,
            Ci := ((y * ext_i / dim) + org_i)
            for(x, 0, dim-1,
                Cr := ((x * ext_r / dim) + org_r)
                cr atPut(size, Cr)
                ci atPut(size, Ci)
                size = size + 1
            )
        )

        zr := cr clone
        zi := ci clone
        zr2 := Vector clone setSize(size)
        zi2 := Vector clone setSize(size)
        temp := Vector clone setSize(size)

        max_iterations repeat(
            temp copy(zr) *= zi

            zr2 copy(zr) square
            zi2 copy(zi) square

            zr copy(zr2) -= zi2 += cr
            zi copy(temp) *= 2  += ci
        )

        self result := zi2 + zr2
    )

    printSet := method(
        writeln("P4\n", dim, " ", dim)
        out := File standardOutput
        i := 0
        dim repeat(
            (dim / 8) repeat(
                pixel := 0
                8 repeat(
                    pixel = pixel shiftLeft(1)
                    if (limit > result at(i), pixel = pixel | 1)
                    i = i + 1
                )
                out write(pixel asCharacter)
            )
            /* if (dim%8!=0, ) */
        )
    )
    
	printSet := method(
        writeln("P4\n", dim, " ", dim)
        out := File standardOutput
        i := 0
        dim repeat(
            (dim / 8) repeat(
                pixel := 0
                8 repeat(
                    pixel = pixel shiftLeft(1)
                    if (result at(i) < limit, pixel = pixel | 1)
                    i = i + 1
                )
                out write(pixel asCharacter)
            )
            /* if (dim%8!=0, ) */
        )
    )
)

MandelbrotSet dim := System args at(1) asNumber
MandelbrotSet do( calc; printSet )

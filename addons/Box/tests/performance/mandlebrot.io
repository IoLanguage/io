#!/usr/bin/env io

MandlebrotSet := Object clone do(
    max_iterations := 1000
	
    calc := method(
    	cr := Vector clone
        ci := Vector clone

	i := 0
	for(y, -39, 39,
	    for(x, -39, 39,
			ci atPut(i, x/40.0)
			cr atPut(i, (y/40.0) -0.5)
			i = i + 1
	    )
	)

	size := cr size

	zi := Vector clone setSize(size)
	zr := Vector clone setSize(size) 
	zr2 := Vector clone setSize(size)
	zi2 := Vector clone setSize(size)
	temp := Vector clone setSize(size)

	for(i, 1, max_iterations,
	    temp copy(zr) *= zi
	    
	    zr2 copy(zr) square
	    zi2 copy(zi) square
	    
	    zr copy(zr2) -= zi2 
	    zr += cr
	    zi copy(temp) *= 2
	    zi += ci
	)

	self result := zi2 + zr2
    )
	
    printSet := method(
		i := 0
		for(y, -39, 39,
			writeln
			for(x, -39, 39,
				r := result at(i) 
				write(if(1 > r and r > -1, "*", " "))
				i = i + 1
			)
		)
    )
)

dt := Date secondsToRun(MandlebrotSet calc)
MandlebrotSet printSet

writeln("\nIo Elapsed " .. dt)

/*
iterator := Object clone do(
    bailout := 16
    max_iterations := 1000
    
    mandelbrot := method(x, y,
	cr := y - 0.5
	ci := x
	zi := 0.0
	zr := 0.0
	bailout := bailout
	
        for(i, 1, max_iterations,
                temp := zr * zi
                zr2 := zr squared
                zi2 := zi squared
                zr := zr2 - zi2 + cr
                zi := (temp * 2) + ci
                if (zi2 + zr2 > bailout, return i)
        )
	return 0
    )

    printSet := method(
	writeln("Rendering...")

	for(y, -39, 39,
	    write("\n")
	    for(x, -39, 39,
		i := mandelbrot(x/40.0, y/40.0)
		write(if(i == 0, "*", " "))
	    )
	)
    )
)

dt := Date secondsToRun(iterator printSet)
writeln("\nIo Elapsed " .. Date secondsToRun(iterator printSet))
*/


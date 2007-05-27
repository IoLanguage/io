#!/usr/bin/env io


/*
MandlebrotApp := GLApp clone do(

	draw := method(
	
	
	)

) run
*/

MandlebrotSet := Object clone do(
    max_iterations := 1000
	
    calc := method(
		size := 79
		cr := Vector clone setSize(size*size)
		ci := cr clone
		zi := cr clone
		zr := cr clone 
		zr2  := cr clone 
		zi2  := cr clone 
		temp := cr clone
		
		ci rangeFill(vector(size, size), 0) 
		ci -= 39	
		ci /= 40
		
		cr rangeFill(vector(size, size), 1) 
		cr -= 39
		cr /= 40
		cr -= .5
		
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
		//self result := (zi2 += zr2)
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
dt2 := Date secondsToRun(MandlebrotSet printSet)

writeln("\n")
writeln("calc time  " .. dt)
writeln("print time " .. dt2)
writeln("total time " .. (dt + dt2))


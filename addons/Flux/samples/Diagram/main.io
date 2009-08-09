#!/usr/bin/env io

Flux

Vector rot90 := method(vector(y, -x, 0))

Application clone do(
    appDidStart := method(
		setTitle("Diagram") 
		mainWindow resizeTo(1024, 768)	
		Background size copy(mainWindow size)
		mainWindow addSubview(Background)
		display
	)
	run
)


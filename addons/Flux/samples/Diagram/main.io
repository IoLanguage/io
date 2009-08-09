#!/usr/bin/env io

Flux

Vector rot90 := method(vector(y, -x, 0))

Application clone do(
    appDidStart := method(
		setTitle("Diagram")
		newSize := Point clone do( x = 1024; y = 768)
		mainWindow reshapeToSize(newSize)
		Background size copy(mainWindow size)
		mainWindow addSubview(Background)
		display
	)
	run
)


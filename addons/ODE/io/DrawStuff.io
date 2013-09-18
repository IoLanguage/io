#!/usr/bin/env io

// drawstuff.io - A simple DrawStuff test program
// (C) ∃teslos 2013
ODE
DrawStuff

DrawStuffApp := DrawStuff clone do(
    appendProto(DrawStuff)
		
	start := method(xyz, hpr,
        xyz := vector(0,0,0)
        hpr := vector(0,0,0)        
		dsGetViewpoint(xyz, hpr)
		dsSetViewpoint(xyz, hpr)
	)
	step := nil
	run := method(
  	writeln("Running run method")
    eventTarget(self)
    self dsSimulationLoop(400,400)
	)
)

//DrawStuffApp clone run
      

#!/usr/bin/env io

// drawstuff.io - A simple DrawStuff test program
// (C) ∃teslos 2013
// Based on dstest in ode-0.12 distribution

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
    a := Number clone
	step := method(paused,
		DS_WOOD := 1
		DS_NONE := 0
        pi := Number constants pi
		if(paused==0, a = a + 0.02)
		if(a > 2*pi, a = a - 2*pi)
		ca := a cos
		sa := a sin
		dsSetTexture(DS_WOOD)
        
		b := if(a > pi, 2*(a-pi), a*2)
        pos := vector(-0.3,0, (0.1*(2*pi*b-b*b)+0.65))
        R := Vector clone setSize(12)
        R atPut(0,ca); R atPut(1,0); R atPut(2,-sa)
        R atPut(4,0 ); R atPut(5,1); R atPut(6,  0)
        R atPut(8,sa); R atPut(9,0); R atPut(10,ca)
        dsSetColor(1.0,0.8,0.6)
		dsDrawSphere(pos,R,0.3)
		dsSetTexture(DS_WOOD)

		// draw the box
		pos := vector(-0.2, 0.8, 0.4)
		sides := vector(0.1,0.4,0.8)
		// define the rotation matrix
		R atPut(0,ca); R atPut(1,-sa); R atPut(2, 0)
		R atPut(4,sa); R atPut(5, ca); R atPut(6, 0)
		R atPut(8, 0); R atPut(9,  0); R atPut(10,1)
        dsSetColor(0.6,0.6,1);
		dsDrawBox(pos, R, sides)

		// draw the cylinder
		r := 0.3
		d := (a*2) cos * 0.4;
		cd := (-d/r) cos
		sd := (-d/r) sin
		pos := vector(-0.2,-1+d,0.3)
        R atPut(0,  0); R atPut(1, 0); R atPut(2,-1)
        R atPut(4,-sd); R atPut(5,cd); R atPut(6, 0)
        R atPut(8, cd); R atPut(9,sd); R atPut(10,0)
        dsSetColor(0.4,1,1)
        dsDrawCylinder(pos,R, 0.8,r)

        // draw capsule
        pos := vector(0,0,0.2)
        R atPut(0,0); R atPut(1,sa); R atPut(2,-ca)
        R atPut(4,0); R atPut(5,ca); R atPut(6, sa)
        R atPut(8,1); R atPut(9, 0); R atPut(10, 0)
        dsSetColor(1,0.9,0.2)
        dsDrawCapsule(pos,R,0.8, 0.2)

	)

	run := method(
    eventTarget(self)
    self dsSimulationLoop(400,400)
	)
)

DrawStuffApp clone run
      

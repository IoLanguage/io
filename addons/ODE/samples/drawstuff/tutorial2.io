#!/usr/bin/env io

# Port of http://pyode.sourceforge.net/tutorials/tutorial2.html
// (C) ∃teslos 2013
// Based on tutorial2.io 
// Based on glballs.io - A simple program (C) 2002 Mike Austin
ODE
DrawStuff

# Create a world object
world := ODEWorld clone
world setGravity(0,0, -9.81)

# Create two bodies
body1 := world Body clone
body1 setMass(ODEMass clone setSphereMass(1, 0.05))
body1 setPosition(0, 1, 2)

body2 := world Body clone
body2 setMass(ODEMass clone setSphereMass(1, 0.05))
body2 setPosition(0, 2, 2)

# Connect body1 with the static environment
jg := world JointGroup clone
j1 := jg Ball clone
j1 attach(body1)
j1 setAnchor(0, 0, 2)

# Connect body2 with body1
j2 := jg Ball clone
j2 attach(body1, body2)
j2 setAnchor(0, 2, 2)


# DrawStuff events object
Screen := DrawStuff clone do(
        appendProto(DrawStuff)
        width ::= 512
        height ::= 512
        
        start := method(xyz, hpr,
                xyz := vector(0,0,0)
                hpr := vector(0,0,0)
                dsGetViewpoint(xyz, hpr)
                dsSetViewpoint(xyz, hpr)
        )

        step := method(paused,
                world step(0.01)
                DS_WOOD := 1
                dsSetTexture(DS_WOOD)
                R := Vector clone setSize(12)
                R atPut(0,1); R atPut(5,1); R atPut(10,1)
                dsSetColor(1.0,0.8,0.6)
                pos1 := body1 position + vector(0.0, 0, 1)
                pos2 := body2 position + vector(0.0, 0, 1)
                dsDrawSphere(pos1, R, 0.3 )
                dsDrawSphere(pos2, R, 0.3 )
        )
       
        run := method(
            eventTarget(self)
            self dsSimulationLoop(width, height)
        )
)

Screen run

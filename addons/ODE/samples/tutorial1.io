#!/usr/bin/env io

# Port of http://pyode.sourceforge.net/tutorials/tutorial1.html

r := DynLib clone setPath("/usr/local/lib/libode.la") open

writeln("r = ", r)
writeln("loaded ode")

# Create a world object
world := ODEWorld clone
world setGravity(0, -9.81, 0)

# Create a body inside the world
body := world Body clone
body setMass(ODEMass clone setSphereMass(1, 0.05))

body setPosition(0, 2, 0)
body addForce(0, 200, 0)

# Do the simulation...
totalTime := 0
dt := 0.04
while(totalTime < 2,
        position := body position
        velocity := body linearVelocity

        writeln("t: ", totalTime, " y: ", position y, " dy: ", velocity y)

        world step(dt)
        totalTime = totalTime + dt
)

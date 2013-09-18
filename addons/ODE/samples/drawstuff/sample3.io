#!/usr/bin/env io


ODE
DrawStuff

DrawStuffSampleApp := DrawStuff clone do(
appendProto(ODE)

step := method(paused,
    radius := 0.4
    dsSetColor(1.0,0.0,0.0)
    pos := vector(0.0,0.0,0.6)
    R := Vector clone setSize(12)
    R atPut(0,1); R atPut(5,1); R atPut(10,1); 
    dsDrawSphere(pos,R,radius)
)

start := method(
    xyz := vector(0.0,-3.0,1.0)
    hpr := vector(90.0,0.0,0.0)
    dsSetViewpoint(xyz, hpr)
)

run := method(
    eventTarget(self)
    dsSimulationLoop(352,288)
)
)

DrawStuffSampleApp run


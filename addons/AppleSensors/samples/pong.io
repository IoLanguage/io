#!/usr/bin/env io


AppleSensors do(
    lastLeft := 0
    maxLeft  := 0
    minLeft  := 1000
    maxRight := 0
    
    getCalibratedLeftLeightSensor := method(
        v  := AppleSensors getLeftLightSensor 
        if(v < 0, v = lastLeft)
        v = v * v
        if (v > maxLeft, maxLeft = v)
        if (v < minLeft, minLeft = v)
        lastLeft = v
        (v - minLeft) / (maxLeft - minLeft)
    )
)

Thing := Object clone do(
    init := method(
        self bounds := Box clone
    )
)

Paddle := Thing clone do(
)

Ball := Thing clone do(
)

Pong := GLApp clone do(
    bposition := vector(25,0)
    bvelocity := vector(2, 2)
    bsize := vector(25, 25)
 
    pposition := vector(25,0)
    psize := vector(40, 140)
    
    draw := method(
        glColor4d(1, 1, 1, 1)

        glPushMatrix
        bposition glTranslate
        bsize drawQuad
        glPopMatrix

        glPushMatrix
        pposition glTranslate
        psize drawQuad       
        glPopMatrix
    )
    
    timer := method(
        
        pposition setY((height - psize y) * AppleSensors getCalibratedLeftLeightSensor)
        
        
        bposition += bvelocity

        if(bposition x + bsize x > width, bvelocity setX(bvelocity x negate))
        if(bposition x < 0, bvelocity setX(bvelocity x negate))
        if(bposition y + bsize y > height, bvelocity setY(bvelocity y negate))
        if(bposition y < 0, bvelocity setY(bvelocity y negate))

 		glutTimerFunc(0, 0)		
        glutPostRedisplay       
    )
) run

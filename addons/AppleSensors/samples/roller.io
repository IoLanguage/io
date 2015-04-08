#!/usr/bin/env io

Ball := Object clone do(
    appendProto(OpenGL)
    position := vector(25,0)
    velocity := vector(2, 2)
    acceleration := vector(0, 0, 0)
    radius := 64
    elasticity := .66
    friction := .995
    
    setBounds := method(bounds,
        self bounds := bounds
        self minPosition := vector(0,0,0)
        self maxPosition := vector(bounds width - radius, bounds height - radius, 0)
    )
    
    
    timeStep := method(
        AppleSensors smsVector(acceleration) 
        acceleration *= -.0005
        
        velocity += acceleration
        velocity *= friction
        position += velocity
        position setZ(0)
        
        if(position x between(0, maxPosition x) not, velocity setX(velocity x negate * elasticity))
        if(position y between(0, maxPosition y) not, velocity setY(velocity y negate * elasticity))
        
        position Max(minPosition) Min(maxPosition)
    )

    quadratic := method(
        self quadratic := gluNewQuadric
    )
    
    display := method(
        glPushMatrix
        position glTranslate
        glTranslated(radius/2, radius/2, 0)
        glColor4d(1, 1, 1, .05)
        gluDisk(quadratic, 0, radius/2, 20, 1)
        glPopMatrix
    )
)

Roller := GLApp clone do(
    ball := Ball clone

    didReshape := method(
        ball setBounds(Box clone setSize(vector(width, height, 0)))
    )
    
    appDidInit := method(
		toggleFullScreen
    )
    
    keyboard := method(
        resend
        backgroundColor glClearColor
		glClear(GL_COLOR_BUFFER_BIT)
    )
    
    display := method(
        if(didInit not, appDidInit; didInit = true)
        if(count < 10,
            backgroundColor glClearColor
            glClear(GL_COLOR_BUFFER_BIT)
		)
        ball display
		glFlush
		glutSwapBuffers
    )
    
    count := 0
    
    timer := method(
        count = count + 1
        ball timeStep
 		glutTimerFunc(0, 0)		
        glutPostRedisplay       
    )
) run

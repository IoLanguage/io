#!/usr/bin/env io

# Port of http://pyode.sourceforge.net/tutorials/tutorial2.html
// (C) 2006 Jonathan Wright
// Based on glballs.io - A simple program (C) 2002 Mike Austin

# Create a world object
world := ODEWorld clone
world setGravity(0, -9.81, 0)

# Create two bodies
body1 := world Body clone
body1 setMass(ODEMass clone setSphereMass(1, 0.05))
body1 setPosition(1, 2, 0)

body2 := world Body clone
body2 setMass(ODEMass clone setSphereMass(1, 0.05))
body2 setPosition(2, 2, 0)

# Connect body1 with the static environment
jg := world JointGroup clone
j1 := jg Ball clone
j1 attach(body1)
j1 setAnchor(0, 2, 0)

# Connect body2 with body1
j2 := jg Ball clone
j2 attach(body1, body2)
j2 setAnchor(1, 2, 0)

appendProto(OpenGL)

# Glut events object
Screen := Object clone do(
        width ::= 512
        height ::= 512
        
        reshape := method( w, h,
                setWidth(w) setHeight(h)

                glMatrixMode( GL_PROJECTION )
                glLoadIdentity()
                gluPerspective( 60, w / h, .5, 10.0 )
                glMatrixMode( GL_MODELVIEW )

                glViewport( 0, 0, w, h )
        )

        display := method(
                glClear( GL_COLOR_BUFFER_BIT | (GL_DEPTH_BUFFER_BIT) )

                glLoadIdentity
                glTranslated( 0, 0, -5 )

                glPushMatrix
                body1 position glTranslate
                glutSolidSphere( .2, 24, 32 )
                glPopMatrix

                glPushMatrix
                body2 position glTranslate
                glutSolidSphere( .2, 24, 32 )
                glPopMatrix

                glDisable( GL_LIGHTING )
                glutWireCube( 3 )
                glEnable( GL_LIGHTING )

                glFlush
                glutSwapBuffers
        )

        timer := method( arg,
                world step(arg / 1000)

                glutPostRedisplay
                glutTimerFunc( arg, arg )
        )
)

glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH )
glutInitWindowSize( Screen width, Screen height )
glutInit
glutCreateWindow("ODE Example")
glutEventTarget( Screen )
glutReshapeFunc
glutDisplayFunc
glutTimerFunc( 0, 10 )

glClearColor( 0, 0, 0, 1 )
glEnable( GL_DEPTH_TEST )
glEnable( GL_LIGHTING )
glEnable( GL_LIGHT0 )

glEnable( GL_BLEND )
glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA )
glEnable( GL_LINE_SMOOTH )
glLineWidth( 1.5 )

glutMainLoop

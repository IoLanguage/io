#!/usr/bin/env io

# Port of http://pyode.sourceforge.net/tutorials/tutorial2.html
// (C) 2006 Jonathan Wright
// Based on glballs.io - A simple program (C) 2002 Mike Austin

# Create a world object
world := ODEWorld clone
world setGravity(0, -9.81, 0)

pos := method(i, (i - 1) * 0.5 + 1)

mass := ODEMass clone setSphereMass(1, 0.01)

# Create two bodies
body1 := world Body clone setMass(mass) setPosition(pos(1), 2, 0)
body2 := world Body clone setMass(mass) setPosition(pos(2), 2, 0)
body3 := world Body clone setMass(mass) setPosition(pos(3), 2, 0)
body4 := world Body clone setMass(mass) setPosition(pos(4), 2, 0)
body5 := world Body clone setMass(mass) setPosition(pos(5), 2, 0)

# Connect body1 with the static environment
jg := world JointGroup clone
j1 := jg Ball clone attach(body1) setAnchor(0, 2, 0)

# Connect body2 with body1
j2 := jg Ball clone attach(body1, body2) setAnchor(pos(1), 2, 0)
j3 := jg Ball clone attach(body2, body3) setAnchor(pos(2), 2, 0)
j4 := jg Ball clone attach(body3, body4) setAnchor(pos(3), 2, 0)
j5 := jg Ball clone attach(body4, body5) setAnchor(pos(4), 2, 0)

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

                list(body1, body2, body3, body4, body5) foreach(body,
                        glPushMatrix
                        body position glTranslate
                        glutSolidSphere( .2, 24, 32 )
                        glPopMatrix
                )

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

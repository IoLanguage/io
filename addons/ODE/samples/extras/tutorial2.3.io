#!/usr/bin/env io

# Port of http://pyode.sourceforge.net/tutorials/tutorial2.html
// (C) 2006 Jonathan Wright
// Based on glballs.io - A simple program (C) 2002 Mike Austin

# Create a world object
world := ODEWorld clone
world setGravity(0, -9.81, 0)

pos := method(i, (i - 1) * 0.5 + 1)

r := 4

mass := ODEMass clone setSphereMass(1, 0.01)

# Create two bodies
body1 := world Body clone setMass(mass) setPosition(0, -pos(1), r)
body2 := world Body clone setMass(mass) setPosition(0, -pos(2), r)
body3 := world Body clone setMass(mass) setPosition(0, -pos(3), r)
body4 := world Body clone setMass(mass) setPosition(0, -pos(4), r)
body5 := world Body clone setMass(mass) setPosition(0, -pos(5), r)

# Connect body1 with the static environment
jg := world JointGroup clone
j1 := jg Hinge clone attach(body1) setAnchor(0, 0, 0) setAxis(0, 1, 0)
j1 setVelocity(5) setMaxForce(40)

# Connect body2 with body1
j2 := jg Ball clone attach(body1, body2) setAnchor(0, -pos(1), r)
j3 := jg Ball clone attach(body2, body3) setAnchor(0, -pos(2), r)
j4 := jg Ball clone attach(body3, body4) setAnchor(0, -pos(3), r)
j5 := jg Ball clone attach(body4, body5) setAnchor(0, -pos(4), r)


bbody1 := world Body clone setMass(mass) setPosition(0, -pos(1), r*2)
bbody2 := world Body clone setMass(mass) setPosition(0, -pos(2), r*2)
bbody3 := world Body clone setMass(mass) setPosition(0, -pos(3), r*2)
bbody4 := world Body clone setMass(mass) setPosition(0, -pos(4), r*2)
bbody5 := world Body clone setMass(mass) setPosition(0, -pos(5), r*2)

# Connect body1 with the static environment
#bj1 := jg Hinge clone attach(bbody1) setAnchor(0, 0, 0) setAxis(0, 1, 0)
#bj1 setVelocity(5) setMaxForce(40)
bj1 := jg Ball clone attach(body5, bbody1) setAnchor(0, -pos(5), r)

# Connect body2 with body1
bj2 := jg Ball clone attach(bbody1, bbody2) setAnchor(0, -pos(1), r*2)
bj3 := jg Ball clone attach(bbody2, bbody3) setAnchor(0, -pos(2), r*2)
bj4 := jg Ball clone attach(bbody3, bbody4) setAnchor(0, -pos(3), r*2)
bj5 := jg Ball clone attach(bbody4, bbody5) setAnchor(0, -pos(4), r*2)


appendProto(GLFW)
appendProto(OpenGL)

draw := method(
    # Get current time
    t := glfwGetTime()
    dt := t - lastTime

    # Get window size
    size := glfwGetWindowSize

    # Make sure that height is non-zero to avoid division by zero
    if(size height < 1, size setHeight(1))

    # Set viewport
    glViewport( 0, 0, size width, size height )

    # Clear color and depth buffers
    glClearColor( 0, 0, 0, 0 )
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

    # Set up projection matrix
    glMatrixMode( GL_PROJECTION )     # Select projection matrix
    glLoadIdentity                    # Start with an identity matrix
    gluPerspective(                   # Set perspective view
        65,                           # Field of view = 65 degrees
        size width / size height,     # Window aspect (assumes square pixels)
        1,                            # Near Z clipping plane
        100                           # Far Z clippling plane
    )

    # Set up modelview matrix
    glMatrixMode( GL_MODELVIEW )      # Select modelview matrix
    glLoadIdentity                    # Start with an identity matrix
    gluLookAt(                        # Set camera position and orientation
        0, 0, 10,                     # Camera position (x,y,z)
        0, 0, 0,                      # View point (x,y,z)
        0, 1, 0                       # Up-vector (x,y,z)
    )

        world step(dt)

        list(body1, body2, body3, body4, body5, bbody1, bbody2, bbody3, bbody4, bbody5) foreach(body,
                glPushMatrix
                body position glTranslate
                glutSolidSphere( .2, 24, 32 )
                glPopMatrix
        )

    lastTime = t
)

glfwInit

glfwOpenWindow(
        640, 480,          # Width and height of window
        8, 8, 8,           # Number of red, green, and blue bits for color buffer
        8,                 # Number of bits for alpha buffer
        24,                # Number of bits for depth buffer (Z-buffer)
        0,                 # Number of bits for stencil buffer
        GLFW_WINDOW        # We want a desktop window (could be GLFW_FULLSCREEN)
)

glfwSetWindowTitle( "My OpenGL program" )
glfwEnable( GLFW_STICKY_KEYS )

glEnable( GL_DEPTH_TEST )
glEnable( GL_LIGHTING )
glEnable( GL_LIGHT0 )

glEnable( GL_BLEND )
glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA )
glEnable( GL_LINE_SMOOTH )

lastTime := glfwGetTime

while(glfwGetKey(GLFW_KEY_ESC) == 0 and glfwGetWindowParam(GLFW_OPENED) != 0,
        draw
        glfwSwapBuffers

        #glfwSleep(1/60)
)

glfwTerminate

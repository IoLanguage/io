#!/usr/bin/env io

# Port of http://glfw.sourceforge.net/tutorials/lesson08/lesson08.html

Range
appendProto(GLFW)
appendProto(OpenGL)

draw := method(
    # Get current time
    t := glfwGetTime()

    # Get window size
    size := glfwGetWindowSize

    # Make sure that height is non-zero to avoid division by zero
    if(size height < 1, size setHeight(1))

    # Set viewport
    glViewport( 0, 0, size width, size height )

    # Clear color and depth buffers
    glClearColor( 0, 0, 0, 0 )
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

    # Calculate field of view as a function of time
    fieldOfView := (0.5 * t) sin * 30 + 50;

    # Set up projection matrix
    glMatrixMode( GL_PROJECTION )     # Select projection matrix
    glLoadIdentity                    # Start with an identity matrix
    gluPerspective(                   # Set perspective view
        fieldOfView,                  # Field of view
        size width / size height,     # Window aspect (assumes square pixels)
        1,                            # Near Z clipping plane
        100                           # Far Z clippling plane
    )

    # Calculate camera position
    camera := vector((0.3 * t) cos * 20, (0.3 * t) sin * 20, t sin + 4)

    # Set up modelview matrix
    glMatrixMode( GL_MODELVIEW )      # Select modelview matrix
    glLoadIdentity                    # Start with an identity matrix
    gluLookAt(                        # Set camera position and orientation
        camera x, camera y, camera z, # Camera position (x,y,z)
        0, 0, 0,                      # View point (x,y,z)
        0, 1, 0                       # Up-vector (x,y,z)
    )

    # Enable Z buffering
    glEnable( GL_DEPTH_TEST )
    glDepthFunc( GL_LEQUAL )

    # Draw a grid
    glColor3f( 0.7, 1, 1 )
    glBegin( GL_LINES )
    (-10) to(10) foreach(i,
        glVertex3f( -10, 0, i )  # Line along X axis
        glVertex3f(  10, 0, i )  # -"-
        glVertex3f( i, 0, -10 )  # Line along Z axis
        glVertex3f( i, 0,  10 )  # -"-
    )
    glEnd

    # Create a GLU quadrics object
    quadric := gluNewQuadric

    # Draw a blue cone in the center
    glPushMatrix
    glRotatef( -90, 1, 0, 0 )
    glColor3f( 0, 0, 1 )
    gluCylinder( quadric, 1.5, 0, 4, 30, 30 )
    glPopMatrix

    # Draw four spheres in the corners o the grid
    glColor3f( 1, 0.2, 0 )
    glPushMatrix
    glTranslatef( -9, 1, -9 )
    gluSphere( quadric, 1, 30, 30 )
    glPopMatrix
    glPushMatrix
    glTranslatef(  9, 1, -9 )
    gluSphere( quadric, 1, 30, 30 )
    glPopMatrix
    glPushMatrix
    glTranslatef( -9, 1,  9 )
    gluSphere( quadric, 1, 30, 30 )
    glPopMatrix
    glPushMatrix
    glTranslatef(  9, 1,  9 )
    gluSphere( quadric, 1, 30, 30 )
    glPopMatrix

    # Destroy the GLU quadrics object
    gluDeleteQuadric( quadric )
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

while(glfwGetKey(GLFW_KEY_ESC) == 0 and glfwGetWindowParam(GLFW_OPENED) != 0,
        draw
        glfwSwapBuffers
)

glfwTerminate

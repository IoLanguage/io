#!/usr/bin/env io

# Port of http://glfw.sourceforge.net/tutorials/lesson02/lesson02.html

appendProto(GLFW)
appendProto(OpenGL)

draw := method(
    # Get current time
    t := glfwGetTime

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

    # Let us draw a triangle!
    glBegin( GL_TRIANGLES )           # Tell OpenGL that we want to draw a triangle
    glVertex3f( -5, -4, 0 )           # First corner of the triangle
    glVertex3f(  5, -4, 0 )           # Second corner of the triangle
    glVertex3f(  0,  4, 0 )           # Third corner of the triangle
    glEnd                             # No more triangles...
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

# Port of http://glfw.sourceforge.net/tutorials/lesson06/lesson06.html

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

    # Enable Z buffering
    glEnable( GL_DEPTH_TEST )
    glDepthFunc( GL_LEQUAL )

    # Rotate the cube about the x and the y axis
    glRotatef( 45 * t, 1, 0, 0 )
    glRotatef( 32 * t, 0, 1, 0 )

    # Draw the cube: six sides with six different colors
    glBegin( GL_QUADS )
    glColor3f( 1, 0, 0 )      # Front side
    glVertex3f(  2.5, -2.5, -2.5 )
    glVertex3f( -2.5, -2.5, -2.5 )
    glVertex3f( -2.5,  2.5, -2.5 )
    glVertex3f(  2.5,  2.5, -2.5 )
    glColor3f( 1, 1, 0 )      # Back side
    glVertex3f( -2.5, -2.5,  2.5 )
    glVertex3f(  2.5, -2.5,  2.5 )
    glVertex3f(  2.5,  2.5,  2.5 )
    glVertex3f( -2.5,  2.5,  2.5 )
    glColor3f( 0, 1, 0 )      # Top
    glVertex3f(  2.5,  2.5, -2.5 )
    glVertex3f( -2.5,  2.5, -2.5 )
    glVertex3f( -2.5,  2.5,  2.5 )
    glVertex3f(  2.5,  2.5,  2.5 )
    glColor3f( 0, 1, 1 )      # Bottom
    glVertex3f( -2.5, -2.5, -2.5 )
    glVertex3f(  2.5, -2.5, -2.5 )
    glVertex3f(  2.5, -2.5,  2.5 )
    glVertex3f( -2.5, -2.5,  2.5 )
    glColor3f( 0, 0, 1 )      # Left
    glVertex3f( -2.5, -2.5, -2.5 )
    glVertex3f( -2.5, -2.5,  2.5 )
    glVertex3f( -2.5,  2.5,  2.5 )
    glVertex3f( -2.5,  2.5, -2.5 )
    glColor3f( 1, 0, 1 )      # Right
    glVertex3f(  2.5, -2.5,  2.5 )
    glVertex3f(  2.5, -2.5, -2.5 )
    glVertex3f(  2.5,  2.5, -2.5 )
    glVertex3f(  2.5,  2.5,  2.5 )
    glEnd
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

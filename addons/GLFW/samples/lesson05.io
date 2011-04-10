#!/usr/bin/env io

# Port of http://glfw.sourceforge.net/tutorials/lesson05/lesson05.html

Range
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
                65,                       # Field of view = 65 degrees
                size width / size height, # Window aspect (assumes square pixels)
                1,                        # Near Z clipping plane
                100                       # Far Z clippling plane
        )

        # Set up modelview matrix
        glMatrixMode( GL_MODELVIEW )      # Select modelview matrix
        glLoadIdentity                    # Start with an identity matrix
        gluLookAt(                        # Set camera position and orientation
                0, 0, 10,                 # Camera position (x,y,z)
                0, 0, 0,                  # View point (x,y,z)
                0, 1, 0                   # Up-vector (x,y,z)
        )

        # **** Draw a circle of points ***

        # Save the current modelview matrix on the stack
        glPushMatrix

        # Translate (move) the points to the upper left of the display
        glTranslatef( -4, 3, 0 )

        # Rotate the points about the z-axis and the x-axis
        glRotatef( 35 * t, 0, 0, 1 )
        glRotatef( 60 * t, 1, 0, 0 )

        # Now draw the points - we use a for-loop to build a circle
        glColor3f( 1, 1, 1 )
        glBegin( GL_POINTS )
        1 to(20) foreach(k,
                glVertex3f((0.31416 * k ) cos * 2,( 0.31416 * k ) sin * 2, 0 )
        )
        glEnd

        # Restore modelview matrix
        glPopMatrix


        # **** Draw a circle of lines ***

        # Save the current modelview matrix on the stack
        glPushMatrix

        # Translate (move) the lines to the upper right of the display
        glTranslatef( 4, 3, 0 )

        # Rotate the points about the z-axis and the x-axis
        glRotatef( 45 * t, 0, 0, 1 )
        glRotatef( 55 * t, 1, 0, 0 )

        # Now draw the lines - we use a for-loop to build a circle
        glBegin( GL_LINE_LOOP )
        1 to(20) foreach(k,
                glColor3f( 1, 0.05 * k, 0 )
                glVertex3f( ( 0.31416 * k ) cos * 2, ( 0.31416 * k ) sin * 2, 0 )
        )
        glEnd

        # Restore modelview matrix
        glPopMatrix


        # **** Draw a disc using trinagles ***

        # Save the current modelview matrix on the stack
        glPushMatrix

        # Translate (move) the triangles to the lower left of the display
        glTranslatef( -4, -3, 0 )

        # Rotate the triangles about the z-axis and the x-axis
        glRotatef( 25 * t, 0, 0, 1 )
        glRotatef( 75 * t, 1, 0, 0 )

        # Now draw the triangles - we use a for-loop to build a disc
        # Since we are building a triangle fan, we also specify a first
        # vertex for the centre point of the disc.
        glBegin( GL_TRIANGLE_FAN )
        glColor3f( 0, 0.5, 1 )
        glVertex3f( 0, 0, 0 )
        0 to(21) foreach(k,
                glColor3f( 0, 0.05 * k, 1 )
                glVertex3f( ( 0.31416 * k ) cos * 2, ( 0.31416 * k ) sin * 2, 0 )
        )
        glEnd

        # Restore modelview matrix
        glPopMatrix


        # **** Draw a disc using a polygon ***

        # Save the current modelview matrix on the stack
        glPushMatrix

        # Translate (move) the polygon to the lower right of the display
        glTranslatef( 4, -3, 0 )

        # Rotate the polygon about the z-axis and the x-axis
        glRotatef( 65 * t, 0, 0, 1 )
        glRotatef( -35 * t, 1, 0, 0 )

        # Now draw the polygon - we use a for-loop to build a disc
        glBegin( GL_POLYGON )
        0 to(20) foreach(k,
                glColor3f( 1, 0, 0.05 * k )
                glVertex3f( ( 0.31416 * k ) cos * 2, ( 0.31416 * k ) sin * 2, 0 )
        )
        glEnd

        # Restore modelview matrix
        glPopMatrix


        # **** Draw a single quad ***

        # Save the current modelview matrix on the stack
        glPushMatrix

        # Rotate the quad about the y-axis
        glRotatef( 60 * t, 0, 1, 0 )

        # Now draw the quad
        glBegin( GL_QUADS )
        glColor3f( 1, 0, 0 )
        glVertex3f( -1.5, -1.5, 0 )
        glColor3f( 1, 1, 0 )
        glVertex3f(  1.5, -1.5, 0 )
        glColor3f( 1, 0, 1 )
        glVertex3f(  1.5,  1.5, 0 )
        glColor3f( 0, 0, 1 )
        glVertex3f( -1.5,  1.5, 0 )
        glEnd

        # Restore modelview matrix
        glPopMatrix
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

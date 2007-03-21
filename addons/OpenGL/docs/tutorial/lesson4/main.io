// NeHe Tutorial #4
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=04
//
// Ported to Io by Steve Dekorte 2003
// 2004-08-01 Updated by Doc O'Leary

Demo := Object clone
Demo appendProto(OpenGL)

// Add new slots to store the spin state.
Demo rtri := 0      // Rotation angle for the triangle. 
Demo rquad := 0     // Rotation angle for the quadrilateral.

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

Demo InitGL := method(
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
)

// A display with a *twist*.  Get it?  Rotate?  Spin? Twist?  Nevermind . . .
Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    glTranslated(-1.5, 0.0, -6.0)
    
    // Draw a triangle
    glRotated(rtri, 0, 1, 0)    // Rotate The Triangle On The Y axis
    glBegin(GL_TRIANGLES)   // Start drawing a triangle
    glColor3d(1, 0, 0)      // Set the color to red
    glVertex3d(0, 1, 0)     // Top
    glColor3d(0, 1, 0)      // Set the color to green
    glVertex3d(1, -1, 0)    // Bottom Right
    glColor3d(0, 0, 1)      // Set the color to blue
    glVertex3d(-1, -1, 0)   // Bottom Left
    glEnd                   // We are done with the triangle
    
    glLoadIdentity              // Reset The Current Modelview Matrix
    glTranslated(1.5, 0, -6)    // Move Right 1.5 Units And Into The Screen 6.0
    
    // Draw a square (quadrilateral)
    glRotated(rquad, 1, 0, 0)   // Rotate The Quad On The X axis 
    glColor3d(0.5, 0.5, 1)      // Set The Color To Blue One Time Only
    glBegin(GL_QUADS)           // Start drawing a 4 sided polygon
    glVertex3d(-1, 1, 0)        // Top Left
    glVertex3d( 1, 1, 0)        // Top Right
    glVertex3d( 1,-1, 0)        // Bottom Right
    glVertex3d(-1,-1, 0)        // Bottom Left
    glEnd
    
    rtri = rtri + .2        // Increase The Rotation Variable For The Triangle
    rquad = rquad - .15     // Decrease The Rotation Variable For The Quad
    
    glutSwapBuffers
)

// Who keeps the animation going?  We do!
Demo timer := method(v,
    self display
    glutTimerFunc(10, v)
)

// Who starts the animation?  We do!
Demo main := method(
    glutInit
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(640, 480)
    glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99")
    glutEventTarget(self)
    
    self InitGL
    
    glutDisplayFunc
    glutReshapeFunc
    glutTimerFunc(10, 0)    // Get your spin on.
    
    glutMainLoop 
)

Demo main
        

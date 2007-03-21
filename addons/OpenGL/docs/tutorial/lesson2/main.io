// NeHe Tutorial #2
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=02
//
// Ported to Io by Steve Dekorte 2003
// 2004-08-01 Updated by Doc O'Leary

Demo := Object clone
Demo appendProto(OpenGL)

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

// Finally, the display method does something.
Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    
    // Move Left 1.5 units and into the screen 6.0 units.
    glTranslated(-1.5, 0.0, -6.0)
    
    // Draw a triangle
    glBegin(GL_TRIANGLES)   // Start drawing a triangle
    glVertex3d( 0,  1, 0)   // Top
    glVertex3d( 1, -1, 0)   // Bottom Right
    glVertex3d(-1, -1, 0)   // Bottom Left
    glEnd                   // We are done with the triangle
    
    // Move Right 3.0 units.
    glTranslated(3.0, 0.0, 0.0)
    
    // Draw a square (quadrilateral)
    glBegin(GL_QUADS)             // Start drawing a 4 sided polygon
    glVertex3d(-1.0, 1.0, 0.0)    // Top Left
    glVertex3d(1.0, 1.0, 0.0)     // Top Right
    glVertex3d(1.0, -1.0, 0.0)    // Bottom Right
    glVertex3d(-1.0, -1.0, 0.0)   // Bottom Left
    glEnd            
    
    glutSwapBuffers
)

Demo main := method(
    glutInit
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(640, 480)
    glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99")
    glutEventTarget(self)
    
    self InitGL
    
    glutDisplayFunc
    glutReshapeFunc
    
    glutMainLoop 
)

Demo main

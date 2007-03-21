// NeHe Tutorial #5
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=05
//
// Ported to Io by Steve Dekorte 2003
// 2004-08-01 Updated by Doc O'Leary

Demo := Object clone
Demo appendProto(OpenGL)

Demo rtri := 0
Demo rquad := 0

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

//  Finally, some real 3D shapes.
Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    glTranslated(-1.5, 0.0, -6.0)

    glRotated(rtri, 0, 1, 0)    // Rotate The Pyramid On It's Y Axis
    glBegin(GL_TRIANGLES)       // Start Drawing The Pyramid
    
    glColor3d(1, 0, 0)      // Red
    glVertex3d(0, 1, 0)     // Top Of Triangle (Front)
    glColor3d(0, 1, 0)      // Green
    glVertex3d(-1, -1, 1)   // Left Of Triangle (Front)
    glColor3d(0, 0, 1)      // Blue
    glVertex3d(1, -1, 1)    // Right Of Triangle (Front)
    
    glColor3d(1, 0, 0)      // Red
    glVertex3d(0, 1, 0)     // Top Of Triangle (Right)
    glColor3d(0, 0, 1)      // Blue
    glVertex3d(1, -1, 1)    // Left Of Triangle (Right)
    glColor3d(0, 1, 0)      // Green
    glVertex3d(1, -1, -1)   // Right Of Triangle (Right)
    
    glColor3d(1, 0, 0)      // Red
    glVertex3d(0, 1, 0)     // Top Of Triangle (Back)
    glColor3d(0, 1, 0)      // Green
    glVertex3d(1, -1, -1)   // Left Of Triangle (Back)
    glColor3d(0, 0, 1)      // Blue
    glVertex3d(-1, -1, -1)  // Right Of Triangle (Back)
    
    glColor3d(1, 0, 0)      // Red
    glVertex3d(0, 1, 0)     // Top Of Triangle (Left)
    glColor3d(0, 0, 1)      // Blue
    glVertex3d(-1, -1, -1)  // Left Of Triangle (Left)
    glColor3d(0, 1, 0)      // Green
    glVertex3d(-1, -1, 1)   // Right Of Triangle (Left)
    glEnd  
    
    
    glLoadIdentity
    glTranslated(1.5, 0, -7)    // Move Right And Into The Screen
    
    glRotated(rquad, 1, 1, 1)   // Rotate The Cube On X, Y & Z
    glBegin(GL_QUADS)           // Start Drawing The Cube
    
    glColor3d(0, 1, 0)      // Set The Color To Green
    glVertex3d( 1, 1,-1)    // Top Right Of The Quad (Top)
    glVertex3d(-1, 1,-1)    // Top Left Of The Quad (Top)
    glVertex3d(-1, 1, 1)    // Bottom Left Of The Quad (Top)
    glVertex3d( 1, 1, 1)    // Bottom Right Of The Quad (Top)
    
    glColor3d(1, 0.5, 0)    // Set The Color To Orange
    glVertex3d( 1,-1, 1)    // Top Right Of The Quad (Bottom)
    glVertex3d(-1,-1, 1)    // Top Left Of The Quad (Bottom)
    glVertex3d(-1,-1,-1)    // Bottom Left Of The Quad (Bottom)
    glVertex3d( 1,-1,-1)    // Bottom Right Of The Quad (Bottom)
    
    glColor3d(1, 0, 0)      // Set The Color To Red
    glVertex3d( 1, 1, 1)    // Top Right Of The Quad (Front)
    glVertex3d(-1, 1, 1)    // Top Left Of The Quad (Front)
    glVertex3d(-1,-1, 1)    // Bottom Left Of The Quad (Front)
    glVertex3d( 1,-1, 1)    // Bottom Right Of The Quad (Front)
    
    glColor3d(1, 1, 0)      // Set The Color To Yellow
    glVertex3d( 1,-1,-1)    // Bottom Left Of The Quad (Back)
    glVertex3d(-1,-1,-1)    // Bottom Right Of The Quad (Back)
    glVertex3d(-1, 1,-1)    // Top Right Of The Quad (Back)
    glVertex3d( 1, 1,-1)    // Top Left Of The Quad (Back)
    
    glColor3d(0, 0, 1)      // Set The Color To Blue
    glVertex3d(-1, 1, 1)    // Top Right Of The Quad (Left)
    glVertex3d(-1, 1,-1)    // Top Left Of The Quad (Left)
    glVertex3d(-1,-1,-1)    // Bottom Left Of The Quad (Left)
    glVertex3d(-1,-1, 1)    // Bottom Right Of The Quad (Left)
    
    glColor3d(1, 0, 1)      // Set The Color To Violet
    glVertex3d( 1, 1,-1)    // Top Right Of The Quad (Right)
    glVertex3d( 1, 1, 1)    // Top Left Of The Quad (Right)
    glVertex3d( 1,-1, 1)    // Bottom Left Of The Quad (Right)
    glVertex3d( 1,-1,-1)    // Bottom Right Of The Quad (Right)
    glEnd                   // Done Drawing The Quad

    rtri = rtri + .2
    rquad = rquad - .15
    
    glutSwapBuffers
)

Demo timer := method(v,
    self display
    glutTimerFunc(10, v)
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
    glutTimerFunc(10, 0)
    
    glutMainLoop 
)

Demo main
      

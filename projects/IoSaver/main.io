// This code was created by Richard Campbell '99 
// Ported to Yindo and later to Io by Steve Dekorte

Demo := Object clone do(
  rtri := 0  // Rotation angle for the triangle. 
  rquad := 0 // Rotation angle for the quadrilateral.
)

Demo parent := OpenGL

Demo InitGL := method(Width, Height,  
   // We call this right after our OpenGL window is created.
    glClearColor(0, 0, 0, 0)  // This Will Clear The Background Color To Black
    glClearDepth(1)           // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS)      // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST)   // Enables Depth Testing
    glShadeModel(GL_SMOOTH)   // Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity            // Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45, Width/Height, 0.1, 100)

    glMatrixMode(GL_MODELVIEW)
)

Demo reshape := method(Width, Height,
    // Reset The Current Viewport And Perspective Transformation
    glViewport(0, 0, Width, Height)  
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45, Width/Height, 0.1, 100)
    glMatrixMode(GL_MODELVIEW)
)

Demo display := method(
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) // Clear The Screen And The Depth Buffer
  glLoadIdentity           // Reset The View
  glTranslated(-1.5,0,-6)  // Move Left And Into The Screen

  glRotated(rtri,0,1,0)    // Rotate The Pyramid On It's Y Axis

  glBegin(GL_TRIANGLES)    // Start Drawing The Pyramid

  glColor4d(1,0,0, 1)      // Red
  glVertex3d( 0, 1, 0)     // Top Of Triangle (Front)
  glColor4d(0,1,0, 1)      // Green
  glVertex3d(-1,-1, 1)     // Left Of Triangle (Front)
  glColor4d(0,0,1, 1)      // Blue
  glVertex3d( 1,-1, 1)

  glColor4d(1,0,0, 1)    // Red
  glVertex3d( 0, 1, 0)   // Top Of Triangle (Right)
  glColor4d(0,0,1, 1)    // Blue
  glVertex3d( 1,-1, 1)   // Left Of Triangle (Right)
  glColor4d(0,1,0, 1)    // Green
  glVertex3d( 1,-1, -1)  // Right 

  glColor4d(1,0,0, 1)    // Red
  glVertex3d( 0, 1, 0)   // Top Of Triangle (Back)
  glColor4d(0,1,0, 1)    // Green
  glVertex3d( 1,-1, -1)  // Left Of Triangle (Back)
  glColor4d(0,0,1, 1)    // Blue
  glVertex3d(-1,-1, -1)  // Right Of 
    
    
  glColor4d(1,0,0, 1)    // Red
  glVertex3d( 0, 1, 0)   // Top Of Triangle (Left)
  glColor4d(0,0,1, 1)    // Blue
  glVertex3d(-1,-1,-1)   // Left Of Triangle (Left)
  glColor4d(0,1,0, 1)    // Green
  glVertex3d(-1,-1, 1)   // Right Of Triangle (Left)
  glEnd  


  glLoadIdentity
  glTranslated(1.5,0,-7) // Move Right And Into The Screen
  glRotated(rquad,1,1,1) // Rotate The Cube On X, Y & Z
  glBegin(GL_QUADS)      // Start Drawing The Cube


  glColor4d(0,1,0, 1)     // Set The Color To Blue
  glVertex3d( 1, 1,-1)    // Top Right Of The Quad (Top)
  glVertex3d(-1, 1,-1)    // Top Left Of The Quad (Top)
  glVertex3d(-1, 1, 1)    // Bottom Left Of The Quad (Top)
  glVertex3d( 1, 1, 1)    // Bottom Right Of The Quad (Top)

  glColor4d(1,0.5,0, 1)   // Set The Color To Orange
  glVertex3d( 1,-1, 1)    // Top Right Of The Quad (Bottom)
  glVertex3d(-1,-1, 1)    // Top Left Of The Quad (Bottom)
  glVertex3d(-1,-1,-1)    // Bottom Left Of The Quad (Bottom)
  glVertex3d( 1,-1,-1)    // Bottom Right Of The Quad (Bottom)

  glColor4d(1,0,0, 1)     // Set The Color To Red
  glVertex3d( 1, 1, 1)    // Top Right Of The Quad (Front)
  glVertex3d(-1, 1, 1)    // Top Left Of The Quad (Front)
  glVertex3d(-1,-1, 1)    // Bottom Left Of The Quad (Front)
  glVertex3d( 1,-1, 1)    // Bottom Right Of The Quad (Front)

  glColor4d(1,1,0, 1)     // Set The Color To Yellow
  glVertex3d( 1,-1,-1)    // Bottom Left Of The Quad (Back)
  glVertex3d(-1,-1,-1)    // Bottom Right Of The Quad (Back)
  glVertex3d(-1, 1,-1)    // Top Right Of The Quad (Back)
  glVertex3d( 1, 1,-1)    // Top Left Of The Quad (Back)

  glColor4d(0,0,1, 1)     // Set The Color To Blue
  glVertex3d(-1, 1, 1)    // Top Right Of The Quad (Left)
  glVertex3d(-1, 1,-1)    // Top Left Of The Quad (Left)
  glVertex3d(-1,-1,-1)    // Bottom Left Of The Quad (Left)
  glVertex3d(-1,-1, 1)    // Bottom Right Of The Quad (Left)

  glColor4d(1,0,1, 1)     // Set The Color To Violet
  glVertex3d( 1, 1,-1)    // Top Right Of The Quad (Right)
  glVertex3d( 1, 1, 1)    // Top Left Of The Quad (Right)
  glVertex3d( 1,-1, 1)    // Bottom Left Of The Quad (Right)
  glVertex3d( 1,-1,-1)    // Bottom Right Of The Quad (Right)
  glEnd                   // Done Drawing The Quad

  // What values to use?  Well, if you have a FAST machine and a FAST 3D Card, then
  // large values make an unpleasant display with flickering and tearing.  I found that
  // smaller values work better, but this was based on my experience.

  rtri  += 0.2      
  // Increase The Rotation Variable For The Triangle

  rquad -= 0.15      
  // Decrease The Rotation Variable For The Quad

  // since this is double buffered, swap the buffers to display what just got drawn. 
  glutSwapBuffers
)

Demo timer := method(
  self display
  glutTimerFunc(10, 0)
)

Demo main := method(
  // Select type of Display mode:   
  //  Double buffer 
  //  RGBA color
  //  Alpha components supported 
  //  Depth buffer

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH)
  glutInit
  glutEventTarget(self) 
  
  // get a 640 x 480 window 
  glutInitWindowSize(640, 480)
  
  // the window starts at the upper left corner of the screen 
  glutInitWindowPosition(0, 0)
  
  glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99")

     // Register the drawing Demo with glut
  glutDisplayFunc
  
  // Register the Demo called when our window is resized.
  glutReshapeFunc

  glutTimerFunc(10, 0)

  // Initialize our window. 
  InitGL(640, 480)

  // Start Event Processing Engine  
  glutMainLoop
)

Demo main
      

// NeHe Tutorial #6
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=06
//
// Ported to Io by Steve Dekorte 2003
// Texture fix by Jeffrey Palmer 2003
// 2004-08-01 Updated by Doc O'Leary

Demo := Object clone
Demo appendProto(OpenGL)

// Add new slots to store the cube orientation.
Demo xrot := 0
Demo yrot := 0
Demo zrot := 0

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

//  Load the texture image during set-up.
Demo InitGL := method(
    // Instead of manual texture creation, we'll use Image features.
    self image := Image clone open(Path with(launchPath, "NeHe.jpg"))

    glEnable(GL_TEXTURE_2D) // Enable Texture Mapping
    
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
)

Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    glTranslated(0, 0, -5)

    glRotated(xrot, 1, 0, 0)    // Rotate The Cube On It's X Axis
    glRotated(yrot, 0, 1, 0)    // Rotate The Cube On It's Y Axis
    glRotated(zrot, 0, 0, 1)    // Rotate The Cube On It's Z Axis

    // In Io we can bind straight from the image.  Nifty!
    image bindTexture   // Select Our Texture
    
    glBegin(GL_QUADS)           // Start Drawing The Cube
    
    // Front Face (note that the texture's corners have to match the quad's corners)
    glTexCoord2d(0, 0); glVertex3d(-1,  1,  1)  // Bottom Left Of The Texture and Quad
    glTexCoord2d(1, 0); glVertex3d( 1,  1,  1)  // Bottom Right Of The Texture and Quad
    glTexCoord2d(1, 1); glVertex3d( 1, -1,  1)  // Top Right Of The Texture and Quad
    glTexCoord2d(0, 1); glVertex3d(-1, -1,  1)  // Top Left Of The Texture and Quad
    
    // Back Face
    glTexCoord2d(1, 0); glVertex3d( 1, -1, -1)  // Bottom Right Of The Texture and Quad
    glTexCoord2d(1, 1); glVertex3d( 1,  1, -1)  // Top Right Of The Texture and Quad
    glTexCoord2d(0, 1); glVertex3d(-1,  1, -1)  // Top Left Of The Texture and Quad
    glTexCoord2d(0, 0); glVertex3d(-1, -1, -1)  // Bottom Left Of The Texture and Quad
    
    // Top Face
    glTexCoord2d(0, 1); glVertex3d( 1,  1, -1)  // Top Left Of The Texture and Quad
    glTexCoord2d(0, 0); glVertex3d( 1,  1,  1)  // Bottom Left Of The Texture and Quad
    glTexCoord2d(1, 0); glVertex3d(-1,  1,  1)  // Bottom Right Of The Texture and Quad
    glTexCoord2d(1, 1); glVertex3d(-1,  1, -1)  // Top Right Of The Texture and Quad
    
    // Bottom Face       
    glTexCoord2d(1, 1); glVertex3d( 1, -1, -1)  // Top Right Of The Texture and Quad
    glTexCoord2d(0, 1); glVertex3d(-1, -1, -1)  // Top Left Of The Texture and Quad
    glTexCoord2d(0, 0); glVertex3d(-1, -1,  1)  // Bottom Left Of The Texture and Quad
    glTexCoord2d(1, 0); glVertex3d( 1, -1,  1)  // Bottom Right Of The Texture and Quad
    
    // Right face
    glTexCoord2d(1, 0); glVertex3d( 1, -1,  1)  // Bottom Right Of The Texture and Quad
    glTexCoord2d(1, 1); glVertex3d( 1,  1,  1)  // Top Right Of The Texture and Quad
    glTexCoord2d(0, 1); glVertex3d( 1,  1, -1)  // Top Left Of The Texture and Quad
    glTexCoord2d(0, 0); glVertex3d( 1, -1, -1)  // Bottom Left Of The Texture and Quad
    
    // Left Face
    glTexCoord2d(0, 0); glVertex3d(-1, -1,  1)  // Bottom Left Of The Texture and Quad
    glTexCoord2d(1, 0); glVertex3d(-1, -1, -1)  // Bottom Right Of The Texture and Quad
    glTexCoord2d(1, 1); glVertex3d(-1,  1, -1)  // Top Right Of The Texture and Quad
    glTexCoord2d(0, 1); glVertex3d(-1,  1,  1)  // Top Left Of The Texture and Quad

    glEnd   // Done Drawing The Quad

    xrot = xrot + 0.3   // X Axis Rotation
    yrot = yrot + 0.2   // Y Axis Rotation
    zrot = zrot + 0.4   // Z Axis Rotation
    
    glutSwapBuffers
)

Demo timer := method(v,
  self display
  glutTimerFunc(10,0)
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
      

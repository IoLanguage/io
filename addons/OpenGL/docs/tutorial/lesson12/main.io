// NeHe Tutorial #12
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=12
//
// Io port by Steve Dekorte, 2003
// 2004-12-28 Updated by Doc O'Leary

/*
	Like the last one, this tutorial is (supposedly) built on #6, but it
	really has more of #8 in it.  You could go back and look at it, but
	it's mostly just the keyboard and texture stuff.
*/

Demo := Object clone
Demo appendProto(OpenGL)

Demo do(
    xrot := 0  
    yrot := 0   
    
    filter := 2 // We'll start with the MipMap
    
    texture := list(1, 2, 3)
    
    // Define the RGB color points for the box rows
    boxcol := list(
        Point clone set(1, 0, 0), 
        Point clone set(1, 0.5, 0), 
        Point clone set(1, 1, 0), 
        Point clone set(0, 1, 0), 
        Point clone set(0, 1, 1)
    )
    
    // Same for the tops
    topcol := list(
        Point clone set(.5,0,0), 
        Point clone set(0.5,0.25,0), 
        Point clone set(0.5,0.5,0), 
        Point clone set(0,0.5,0), 
        Point clone set(0,0.5,0.5)
    )
)

// Create the box and top display lists
Demo BuildLists := method(
    listOffset := glGenLists(2)

    self box := listOffset + 0
    glNewList(box, GL_COMPILE)
        glBegin(GL_QUADS)
        // Bottom Face
        glTexCoord2d(1, 1) glVertex3d(-1, -1, -1)
        glTexCoord2d(0, 1) glVertex3d( 1, -1, -1)
        glTexCoord2d(0, 0) glVertex3d( 1, -1,  1)
        glTexCoord2d(1, 0) glVertex3d(-1, -1,  1)
        // Front Face
        glTexCoord2d(0, 0) glVertex3d(-1, -1,  1)
        glTexCoord2d(1, 0) glVertex3d( 1, -1,  1)
        glTexCoord2d(1, 1) glVertex3d( 1,  1,  1)
        glTexCoord2d(0, 1) glVertex3d(-1,  1,  1)
        // Back Face
        glTexCoord2d(1, 0) glVertex3d(-1, -1, -1)
        glTexCoord2d(1, 1) glVertex3d(-1,  1, -1)
        glTexCoord2d(0, 1) glVertex3d( 1,  1, -1)
        glTexCoord2d(0, 0) glVertex3d( 1, -1, -1)
        // Right face
        glTexCoord2d(1, 0) glVertex3d( 1, -1, -1)
        glTexCoord2d(1, 1) glVertex3d( 1,  1, -1)
        glTexCoord2d(0, 1) glVertex3d( 1,  1,  1)
        glTexCoord2d(0, 0) glVertex3d( 1, -1,  1)
        // Left Face
        glTexCoord2d(0, 0) glVertex3d(-1, -1, -1)
        glTexCoord2d(1, 0) glVertex3d(-1, -1,  1)
        glTexCoord2d(1, 1) glVertex3d(-1,  1,  1)
        glTexCoord2d(0, 1) glVertex3d(-1,  1, -1)
        glEnd
    glEndList
    
    self top := listOffset + 1
    glNewList(top, GL_COMPILE)
        glBegin(GL_QUADS)
        // Top Face
        glTexCoord2d(0, 1) glVertex3d(-1,  1, -1)
        glTexCoord2d(0, 0) glVertex3d(-1,  1,  1)
        glTexCoord2d(1, 0) glVertex3d( 1,  1,  1)
        glTexCoord2d(1, 1) glVertex3d( 1,  1, -1)
        glEnd
    glEndList
)

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

// The only thing changed here is the use of the cube texture.
// I know the original only used one texture, but there's little harm
// in keeping all 3, and the fewer code changes the better.
Demo LoadGLTextures := method(
    // Load the image we want to use as a texture.
    image := Image clone open(Path with(System launchPath, "cube.jpg"))
    
    glGenTextures(3, texture)
    
    glBindTexture(GL_TEXTURE_2D, texture at(0))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image width, image height, 0, GL_RGB, GL_UNSIGNED_BYTE, image data)
    
    glBindTexture(GL_TEXTURE_2D, texture at(1))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image width, image height, 0, GL_RGB, GL_UNSIGNED_BYTE, image data)
    
    glBindTexture(GL_TEXTURE_2D, texture at(2))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST)
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image width, image height, GL_RGB, GL_UNSIGNED_BYTE, image data)
)

// We add code to build the cubes, light them, and color them.
Demo InitGL := method(  
    self LoadGLTextures
    
    self BuildLists // Jump To The Code That Creates Our Display Lists
    
    glEnable(GL_TEXTURE_2D)
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)

    glEnable(GL_LIGHT0)         // Quick And Dirty Lighting
    glEnable(GL_LIGHTING)       // Enable Lighting
    glEnable(GL_COLOR_MATERIAL) // Enable Material Coloring

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
)

//  Allow rotation of the cubes.
Demo keyboard := method(key, x, y,   
    key := key asCharacter
    
    if(key == "f",  filter = filter + 1; if (filter > 2, filter = 0))
    
    // WASD is used instead of the arrow keys
    if(key == "a", yrot = yrot - 0.2)
    if(key == "d", yrot = yrot + 0.2)
    if(key == "w", xrot = xrot - 0.2)
    if(key == "s", xrot = xrot + 0.2)
)

// Draw the cubes using the display lists
Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) 
    
    glBindTexture(GL_TEXTURE_2D, texture at(filter))
    
    for(yloop, 1, 5,
        for(xloop, 0, yloop-1,
            glLoadIdentity 
            glTranslated(
                1.4 + (xloop * 2.8) - (yloop * 1.4),
                ((6 - yloop) * 2.4) -7,
                -20
            )
            glRotated(45 - (2 * yloop) + xrot, 1, 0, 0)
            glRotated(45 + yrot, 0, 1, 0)
            
            boxcol at(yloop-1) glColor
            glCallList(box)
            topcol at(yloop-1) glColor
            glCallList(top)
        )
    )
    
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
    glutKeyboardFunc
    
    glutMainLoop 
)


Demo main

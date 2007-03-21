// NeHe Tutorial #8
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=08
//
// Io port by Steve Dekorte, 2001
// 2004-08-01 Updated by Doc O'Leary

Demo := Object clone
Demo appendProto(OpenGL)

// Add blend to support basic alpha.
Demo do(
    light := 0
    blend := 0  // Blending OFF/ON
    
    xrot := 0
    yrot := 0
    xspeed := 1
    yspeed := 1
    
    z := -5
    
    LightAmbient  := list(.5, .5, .5, 1)
    LightDiffuse  := list(1, 1, 1, 1)
    LightPosition := list(0, 0, 2, 1)
    
    filter := 0
    
    texture := list(1, 2, 3)
)

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

// The only thing changed here is the use of the glass texture.
Demo LoadGLTextures := method(
    // Load the image we want to use as a texture.
    image := Image clone open(Path with(launchPath, "glass.jpg"))
    
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

// Set up support for blending.
Demo InitGL := method(
    self LoadGLTextures
    
    glEnable(GL_TEXTURE_2D)
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse)
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition)
    glEnable(GL_LIGHT1)
    
    glColor4f(1, 1, 1, .5)              // Full Brightness, 50% Alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE)   // Blending Function For Translucency Based On Source Alpha Value
)

// Need to map b key to blend functionality.
Demo keyboard := method(key, x, y,   
    key := key asCharacter
    
    if(key == "l", 
        light = light toggle
        if(light == 0, glDisable(GL_LIGHTING), glEnable(GL_LIGHTING))
    )
    
    if(key == "f",  filter = filter + 1; if (filter > 2, filter = 0))
    
    // Toggle the blending
    if(key == "b", 
        blend = blend toggle
        if(blend == 0,
            glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST)
        ,
            glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST)
        )
    )
    
    if(key == "a", z = z + 0.2)
    if(key == "z", z = z - 0.2)
    if(key == "s", xspeed = xspeed + 0.1)
    if(key == "x", xspeed = xspeed - 0.1)
    if(key == "w", yspeed = yspeed + 0.1)
    if(key == "q", yspeed = yspeed - 0.1)
)

Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    glTranslated(0,0,z)
    
    glRotated(xrot, 1, 0, 0)
    glRotated(yrot, 0, 1, 0)
    
    glBindTexture(GL_TEXTURE_2D, texture at(filter))

    glBegin(GL_QUADS)
    
    // Front Face
    glNormal3d( 0, 0, 1)
    glTexCoord2d(0, 0); glVertex3d(-1, -1,  1)
    glTexCoord2d(1, 0); glVertex3d( 1, -1,  1)
    glTexCoord2d(1, 1); glVertex3d( 1,  1,  1)
    glTexCoord2d(0, 1); glVertex3d(-1,  1,  1)
    // Back Face
    glNormal3d( 0, 0,-1)
    glTexCoord2d(1, 0); glVertex3d(-1, -1, -1)
    glTexCoord2d(1, 1); glVertex3d(-1,  1, -1)
    glTexCoord2d(0, 1); glVertex3d( 1,  1, -1)
    glTexCoord2d(0, 0); glVertex3d( 1, -1, -1)
    // Top Face
    glNormal3d( 0, 1, 0)
    glTexCoord2d(0, 1); glVertex3d(-1,  1, -1)
    glTexCoord2d(0, 0); glVertex3d(-1,  1,  1)
    glTexCoord2d(1, 0); glVertex3d( 1,  1,  1)
    glTexCoord2d(1, 1); glVertex3d( 1,  1, -1)
    // Bottom Face
    glNormal3d( 0,-1, 0)
    glTexCoord2d(1, 1); glVertex3d(-1, -1, -1)
    glTexCoord2d(0, 1); glVertex3d( 1, -1, -1)
    glTexCoord2d(0, 0); glVertex3d( 1, -1,  1)
    glTexCoord2d(1, 0); glVertex3d(-1, -1,  1)
    // Right face
    glNormal3d( 1, 0, 0)
    glTexCoord2d(1, 0); glVertex3d( 1, -1, -1)
    glTexCoord2d(1, 1); glVertex3d( 1,  1, -1)
    glTexCoord2d(0, 1); glVertex3d( 1,  1,  1)
    glTexCoord2d(0, 0); glVertex3d( 1, -1,  1)
    // Left Face
    glNormal3d(-1, 0, 0)
    glTexCoord2d(0, 0); glVertex3d(-1, -1, -1)
    glTexCoord2d(1, 0); glVertex3d(-1, -1,  1)
    glTexCoord2d(1, 1); glVertex3d(-1,  1,  1)
    glTexCoord2d(0, 1); glVertex3d(-1,  1, -1)
    
    glEnd
  
    xrot = xrot + xspeed
    yrot = yrot + yspeed
    
    glutSwapBuffers
)

Demo timer := method(v, 
  self display
  glutTimerFunc(10, 0)
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

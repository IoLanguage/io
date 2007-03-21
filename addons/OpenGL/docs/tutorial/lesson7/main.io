// NeHe Tutorial #7
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=07
//
// Ported to Io by Steve Dekorte 2003
// 2004-08-01 Updated by Doc O'Leary

Demo := Object clone
Demo appendProto(OpenGL)

// Add new slots to store the crate state.
// Since there are so many, we'll stick them all inside a do() block.
Demo do(
    light := 0  // Lighting ON/OFF
    
    xrot := 0     // X Rotation
    yrot := 0     // Y Rotation
    xspeed := 1   // X Rotation Speed
    yspeed := 1   // Y Rotation Speed
    
    z := -5       // Depth Into The Screen
    
    LightAmbient  := list(.5, .5, .5, 1)  // Ambient Light Values
    LightDiffuse  := list(1, 1, 1, 1)     // Diffuse Light Values
    LightPosition := list(0, 0, 2, 1)     // Light Position
    
    filter := 0   // Which Filter To Use
    
    texture := list(1, 2, 3) // Storage for 3 textures
)

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

// We now get around to doing textures the complex way.
Demo LoadGLTextures := method(
    // Load the image we want to use as a texture.
    image := Image clone open(Path with(launchPath, "texture.jpg"))
    
    glGenTextures(3, texture)   // Create Three Textures
    
    // Create Nearest Filtered Texture
    glBindTexture(GL_TEXTURE_2D, texture at(0))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image width, image height, 0, GL_RGB, GL_UNSIGNED_BYTE, image data)
    
    // Create Linear Filtered Texture
    glBindTexture(GL_TEXTURE_2D, texture at(1))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image width, image height, 0, GL_RGB, GL_UNSIGNED_BYTE, image data)
    
    // Create MipMapped Texture
    glBindTexture(GL_TEXTURE_2D, texture at(2))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST)
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image width, image height, GL_RGB, GL_UNSIGNED_BYTE, image data)
)

//  Set up the lighting and create multiple textures.
Demo InitGL := method(
    // We got around it in the last lesson, but we need to build the textures this time.
    self LoadGLTextures
    
    glEnable(GL_TEXTURE_2D)
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient)      // Setup The Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse)      // Setup The Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition)    // Position The Light
    glEnable(GL_LIGHT1)                                 // Enable Light One
)

//  Add keyboard control.
//  Note that many keys are different than the ones used in the site's code.
Demo keyboard := method(key, x, y,   
    key := key asCharacter
    
    // Toggle the lighting
    if(key == "l", 
        light = light toggle
        if(light == 0, glDisable(GL_LIGHTING), glEnable(GL_LIGHTING))
    )
    
    // Switch between filters
    if(key == "f",  filter = filter + 1; if (filter > 2, filter = 0))
    
    if(key == "a", z = z + 0.2)             // Move towards the crate
    if(key == "z", z = z - 0.2)             // Move away from the crate
    if(key == "s", xspeed = xspeed + 0.1)   // Increase xspeed
    if(key == "x", xspeed = xspeed - 0.1)   // Decrease xspeed
    if(key == "w", yspeed = yspeed + 0.1)   // Increase yspeed
    if(key == "q", yspeed = yspeed - 0.1)   // Decrease yspeed
)

Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    glTranslated(0,0,z)   // Translate Into/Out Of The Screen By z
    
    glRotated(xrot, 1, 0, 0)
    glRotated(yrot, 0, 1, 0)
    
    glBindTexture(GL_TEXTURE_2D, texture at(filter))    // Select A Texture Based On filter

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
  
    xrot = xrot + xspeed    // Add xspeed To xrot
    yrot = yrot + yspeed    // Add yspeed To yrot
    
    glutSwapBuffers
)

Demo timer := method(v, 
  self display
  glutTimerFunc(10, 0)
)

//  Adding keyboard control.
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
    glutKeyboardFunc        // Set up the keyboard callback method.
    
    glutMainLoop 
)


Demo main

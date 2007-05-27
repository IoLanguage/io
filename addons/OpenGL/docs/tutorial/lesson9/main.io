// NeHe Tutorial #9
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=09
//
// Io port by Steve Dekorte, 2001
// 2004-10-03 Updated by Doc O'Leary

/*
    Starting with Lesson 9, the tutorials don't really build upon one
    another at the source level.  Hopefully you have an understanding
    of both Io and OpenGL by this point that it doesn't cause too much
    confusion!
*/

// Create the prototype for all stars.
Star := Object clone do(
    r := 0      // Red Color Component
    g := 0      // Green Color Component
    b := 0      // Blue Color Component
    dist := 0   // Distance From Center
    angle := 0  // Current Angle
)
  
// Because we have the luxury of objects, we'll put the bulk of the
// setup code for a star here instead of both InitGL and display.
// Note that we're also using floating point for colors and not bytes,
// since our random number generator works just fine.
Star updateWithOffset := method(offset,
    self r = Random value
    self g = Random value
    self b = Random value
    self dist = self dist + offset
    
    return self
)
  
Demo := Object clone
Demo appendProto(OpenGL)

// We need to set up an entirely new environment.
Demo do(
    twinkle := 0        // Twinkling Stars
    
    num := 50           // Number Of Stars To Draw
    stars := List clone // Container for all the stars

    zoom := -15         // Viewing Distance Away From Stars
    tilt := 90          // Tilt The View
    spin := 0           // Spin Twinkling Stars
    
    texture := list(1)  // Storage For One Texture
)

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45, w/h, 0.1, 100)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

// The only thing changed here is the use of the star texture.
// Note that while in Lesson 6 we simply used the texture from the raw
// Image, we cannot do that here because we are going to color it later.
Demo LoadGLTextures := method(
    image := Image clone open(Path with(launchPath, "star.jpg"))
    
    glGenTextures(1, texture)
    
    glBindTexture(GL_TEXTURE_2D, texture at(0))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image width, image height, 0, GL_RGB, GL_UNSIGNED_BYTE, image data)
)

//  Create all the stars.
Demo InitGL := method(
    self LoadGLTextures
    
    glEnable(GL_TEXTURE_2D)
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE)
    glEnable(GL_BLEND) 
    
    for(loop, 0, num - 1,
        stars append(Star clone updateWithOffset(5 * loop / num))
    )
)

// All mappings are altered for the new environment.
Demo keyboard := method(key, x, y,
    key := key asCharacter
    
    if(key == "t", twinkle = twinkle toggle)
    
    if(key == "a", zoom = zoom + 0.2)
    if(key == "z", zoom = zoom - 0.2)
    if(key == "s", tilt = tilt + 0.5)
    if(key == "x", tilt = tilt - 0.5)
)

// Drawing code almost entirely revised as well.
Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)  
    glBindTexture(GL_TEXTURE_2D, texture at(0))
    
    stars foreach(i, aStar,
        glLoadIdentity   
        glTranslated(0, 0, zoom) 
        glRotated(tilt, 1, 0, 0)  
        
        glRotated(aStar angle, 0, 1, 0) 
        glTranslated(aStar dist, 0, 0)  
        
        glRotated(aStar angle negate, 0, 1, 0)
        glRotated(tilt negate, 1, 0, 0)   
        
        if(twinkle == 1, 
            tStar := stars at(num - i - 1)
            glColor4d(tStar r, tStar g, tStar b, 1)
            glBegin(GL_QUADS) // Begin Drawing The Textured Quad
                glTexCoord2d(0, 0); glVertex3d(-1,-1, 0)
                glTexCoord2d(1, 0); glVertex3d( 1,-1, 0)
                glTexCoord2d(1, 1); glVertex3d( 1, 1, 0)
                glTexCoord2d(0, 1); glVertex3d(-1, 1, 0)
            glEnd // Done Drawing The Textured Quad
        )
        
        glRotated(spin, 0, 0, 1)
        glColor4d(aStar r, aStar g, aStar b, 1)
        glBegin(GL_QUADS) // Begin Drawing The Textured Quad
            glTexCoord2d(0, 0); glVertex3d(-1,-1, 0)
            glTexCoord2d(1, 0); glVertex3d( 1,-1, 0)
            glTexCoord2d(1, 1); glVertex3d( 1, 1, 0)
            glTexCoord2d(0, 1); glVertex3d(-1, 1, 0)
        glEnd // Done Drawing The Textured Quad
        
        spin = spin + .01
        aStar angle = aStar angle + (i / num)  
        aStar dist = aStar dist - 0.01   
        
        if (aStar dist < 0, aStar updateWithOffset(5))
    )
    
    glutSwapBuffers
)

// We use a faster timer than before, just for fun.
Demo timer := method(v, 
    self display
    glutTimerFunc(1, 0)
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
    glutTimerFunc(10,0)
    glutKeyboardFunc
    
    glutMainLoop
)


Demo main

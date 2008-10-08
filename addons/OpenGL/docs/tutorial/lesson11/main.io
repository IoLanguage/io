// NeHe Tutorial #11
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=11
//
// Io port by Steve Dekorte, 2003
// 2004-12-28 Updated by Doc O'Leary

/*
	This tutorial is (supposedly) built on #6, so go back and look at that for a
	refresher. As usual, only the changes will be documented.
*/

// Taken from tutorial #10.
Number degreesToRadians := method(
    return self * .0174532925
)

Demo := Object clone
Demo appendProto(OpenGL)

Demo xrot := 0
Demo yrot := 0
Demo zrot := 0

// The grid we'll be waving
Demo points := List clone

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

//  Load the texture image and set up the points grid during set-up.
Demo InitGL := method(
    self image := Image clone open(Path with(System launchPath, "tim.jpg"))
    
    glEnable(GL_TEXTURE_2D)    
    
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    
    glPolygonMode(GL_BACK, GL_FILL)     // Back Face Is Filled In
    glPolygonMode(GL_FRONT, GL_LINE)    // Front Face Is Drawn With Lines
    
    for(x, 0, 44,
        l := List clone
        for(y, 0, 44,
            p := Point clone
            p setX(x/5 - 4.5)
            p setY(y/5 - 4.5)
            p setZ(((x/5) * 40) degreesToRadians sin)
            l append(p)
        )
        points append(l)
    )
)

//  Draw and wave the flag
Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    glTranslated(0, 0, -12) // Translate 12 Units Into The Screen

    glRotated(xrot, 1, 0, 0)
    glRotated(yrot, 0, 1, 0)
    glRotated(zrot, 0, 0, 1)

    image bindTexture
    
    glBegin(GL_QUADS)
    
    // Here we loop through the grid and map the corresponding portion
    // of our texture to it.
    for(x, 0, 43,
        for(y, 0, 43,
            float_x  := x/44
            float_y  := y/44
            float_xb := 1*(x+1)/44
            float_yb := 1*(y+1)/44
            
            glTexCoord2d(float_x, float_y)
            points at(x) at(y) glVertex
            
            glTexCoord2d(float_x, float_yb)
            points at(x) at(y+1) glVertex
            
            glTexCoord2d(float_xb, float_yb)
            points at(x+1) at(y+1) glVertex
            
            glTexCoord2d(float_xb, float_y)
            points at(x+1) at(y) glVertex
        )
    )
    
    glEnd
    
    // Again we loop through the grid, this time to shift the z values,
    // which gives the wave effect.
    for(y, 0, 44,
        hold := points at(0) at(y) z
        for(x, 0, 43, points at(x) at(y) setZ(points at(x+1) at(y) z))
        points at(44) at(y) setZ(hold)
    )
    
    xrot = xrot + 0.3
    yrot = yrot + 0.2
    zrot = zrot + 0.4
    
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

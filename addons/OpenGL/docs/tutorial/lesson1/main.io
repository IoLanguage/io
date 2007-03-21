// NeHe Tutorial #1
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=01
//
// Ported to Io by Steve Dekorte 2003
// 2004-08-01 Updated by Doc O'Leary

/*
    This code shows how to get OpenGL going with Io.  Support is provided by
    the OpenGL object, which is in the ioDesktop binary.  Simply run it with
    this file as the argument, and you should get a boring and
    empty-yet-stylishly-black GLUT window.
    
    For the most part, the lessons translate fairly directly from the given
    C code over to Io.  For the purposes of clarity, only changed code will
    be documented, and then only extensively if there is a significant
    difference between the site's code and the code ported to Io.
*/

// While we could technically clone OpenGL itself, it is really a singleton.
// To avoid adding all these methods to it, we'll simply make it our parent.
Demo := Object clone
Demo appendProto(OpenGL)

// This method handles what ReSizeGLScene does in the original tutorial.
Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

//  Same as the InitGL of the original.
Demo InitGL := method(
    glShadeModel(GL_SMOOTH)       // Enables Smooth Shading
    glClearColor(0, 0, 0, 0)      // Black Background
    glClearDepth(1)               // Enables Clearing Of The Depth Buffer
    glEnable(GL_DEPTH_TEST)       // Enables Depth Testing
    glDepthFunc(GL_LEQUAL)        // The Type Of Depth Test To Do
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)   // Really Nice Perspective Calculations
)

// This method handles what DrawGLScene does in the original tutorial.
Demo display := method(
    // Clear The Screen And The Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    
    glutSwapBuffers     // Because GLUT double buffers.
)

//  To avoid all the window consruction steps, we just use GLUT.
//  This is same as is done in the main() for Mac OS X:
//  http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Mac_OS_X
Demo main := method(
    glutInit
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(640, 480)
    glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99")
    glutEventTarget(self)
    
    self InitGL
    
    // Call our display and reshape methods by default.
    glutDisplayFunc
    glutReshapeFunc
    
    glutMainLoop // Start Event Processing Engine  
)

// And, of course, we have to call main to get everything going.
Demo main

// NeHe Tutorial #13
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=13
//
// 2004-12-28 Created by Doc O'Leary

/*
    This lesson is (supposedly) built on tutorial #1, but I say it's more like #4.
    Whether or not you consider this one a "cheat" is up to you, but Io doesn't really
    bother to deal with bitmapped fonts.  We'll just use the standard Font object and
    adapt it to drawing done in this lesson.
*/

Demo := Object clone
Demo appendProto(OpenGL)

// Add new slots to store the scene state.
Demo base := Font clone
Demo cnt1 := 0
Demo cnt2 := 0

// Set up the base font.
Demo BuildFont := method(
//base = glGenLists(96)   // Storage For 96 Characters
//wglUseFontBitmaps(hDC, 32, 96, base)  // Builds 96 Characters Starting At Character 32
    base open("IoResources/Library/Fonts/Free/Mono/Bold.ttf")
    base setPixelSize(24)
  glEnable(GL_BLEND)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
)

// Print a string.  We don't bother with variable arguments because an append is so easy.
Demo glPrint := method(text,
//glPushAttrib(GL_LIST_BIT) // Pushes The Display List Bits
//glListBase(base - 32) // Sets The Base Character to 32
//glCallLists(strlen(text), GL_UNSIGNED_BYTE, text) // Draws The Display List Text
//glPopAttrib() // Pops The Display List Bits
    glTranslated(-0.45 + (0.05 * (cnt1 cos)), 0.35 * (cnt2 sin), 0);
    base drawString("Active OpenGL Text With NeHe - " .. cnt1 asString(7, 2))
)

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)     
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45.0, w / h, 0.1, 100.0)
//gluOrtho2D(0, w, 0, h)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

// Added the font set-up call.
Demo InitGL := method(
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    
    self BuildFont
)

// Add the code to display the text.
Demo display := method(
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    glTranslated(0, 0, -1)   // Move One Unit Into The Screen
    
    // Pulsing Colors Based On Text Position
    glColor3d(cnt1 cos, cnt1 sin, 1 - (0.5 * ((cnt1 + cnt2) cos)))
    
    // Position The Text On The Screen
    glRasterPos2d(-0.45 + (0.05 * (cnt1 cos)), 0.35 * (cnt2 sin));
    glPrint("Active OpenGL Text With NeHe - " .. cnt1 asString(7, 2))
    
    cnt1 = cnt1 + 0.051 // Increase The First Counter
    cnt2 = cnt2 + 0.005 // Increase The Second Counter
    
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

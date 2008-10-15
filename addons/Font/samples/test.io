#!/usr/bin/env io

monoFont  := Font clone open("../../Flux/resources/fonts/Vera/Mono/Bold.ttf") setPixelSize(12)
writeln("top1")
sansFont  := Font clone open("../../Flux/resources/fonts/Vera/Sans/Normal.ttf") setPixelSize(12)
serifFont := Font clone open("../../Flux/resources/fonts/Vera/Serif/Normal.ttf") setPixelSize(12)

writeln("top")

Screen := Object clone
Screen appendProto(OpenGL)
Screen string := "Io TrueType 123!@#"
Screen reshape := method(w, h, 
  self width := w
  self height := h
  glViewport(0,0,w,h)
  glLoadIdentity
  gluOrtho2D(0,w,0,h)
  
  monoFont  setPixelSize(width/15)
  sansFont  setPixelSize(width/15)
  serifFont setPixelSize(width/15)
  
  glutPostRedisplay
)

Screen display := method(
  glClearColor(1, 1, 1, 1)
  glClear(GL_COLOR_BUFFER_BIT)

  x := width/2 - monoFont widthOfString(string) / 2
  y := height/2 + monoFont pixelSize * 1.5
  x = x ceil
  y = y ceil

  //y = y + monoFont pixelSize + 10
  glPushMatrix
  glTranslated(x, y, 0)
  glColor4d(0,0,0,1)
  monoFont drawString(string)
  glPopMatrix

  glPushMatrix
  y = y - sansFont pixelSize *1.5
  glTranslated(x, y, 0)
  glColor4d(0,.7,0,1)
  sansFont drawString(string)
  glPopMatrix

  y = y - serifFont pixelSize *1.5
  glPushMatrix
  glTranslated(x, y, 0)
  glColor4d(0,0,1,1)
  serifFont drawString(string)
  glPopMatrix

  glFlush
  glutSwapBuffers
)

Screen mouse := method(button, state, x, y,
  if (state == 0,
    if ( y < 50, 
      v := Number random(0,1)
      glColor4d(v, 0, 1 - v, 1)
    )
    self display
  )
)

Screen special := method(c, x, y,

  if (c == GLUT_KEY_UP, 
    monoFont  setPixelSize(monoFont  pixelSize + 1)
    sansFont  setPixelSize(sansFont  pixelSize + 1)
    serifFont setPixelSize(serifFont pixelSize + 1)
  )

  if (c == GLUT_KEY_DOWN, 
    monoFont  setPixelSize(monoFont  pixelSize - 1)
    sansFont  setPixelSize(sansFont  pixelSize - 1)
    serifFont setPixelSize(serifFont pixelSize - 1)
  )
  
  display
)

Screen run := method(
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(512, 512)
  glutInit
  glutCreateWindow("Font Test")
  glutEventTarget(Screen)
  glutDisplayFunc
  glutMouseFunc
  glutReshapeFunc
  glutSpecialFunc
  
  glColor4d(0,0,0,1)
  glEnable(GL_BLEND)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
  glutMainLoop
)

Screen run



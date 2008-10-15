#!/usr/bin/env io

sansFontBig     := Font clone open("../../Flux/resources/fonts/Free/Sans/Bold.ttf") setPixelSize(200)
sansFontMedium  := Font clone open("../../Flux/resources/fonts/Free/Sans/Bold.ttf") setPixelSize(25)

Screen := Object clone
Screen appendProto(OpenGL)
Screen timerCount := 0
Screen divisions := 300
Screen reshape := method(w, h, 
  self width := w
  self height := h
  glViewport(0,0,w,h)
  glLoadIdentity
  gluOrtho2D(0,w,0,h) 
  //write((height/4) min(263), "\n")
  sansFontBig setPixelSize((height * .3) min(263))
  sansFontMedium setPixelSize(height/15)
  glutPostRedisplay
)

Screen drawCentered := method(text, font,
  glPushMatrix
  glTranslated(- font widthOfString(text)/2, 0, 0)
  font drawString(text)
  glPopMatrix
)

Screen display := method(
  glClearColor(1, 1, 1, 1)
  glClear(GL_COLOR_BUFFER_BIT)

  glPushMatrix
  glTranslated(width/2, height/2.3, 0)
    glColor4d(0,0,0,1)
    self drawCentered("Io", sansFontBig)
  glTranslated(0, - height / 8, 0)
    glColor4d(.7,.7,.7,1)
   self drawCentered("a programming language", sansFontMedium)
  glPopMatrix

  glPushMatrix
    glColor4d(.8,.8,.8,1)
    glRectd(0, 0, width*timerCount/divisions, 20)
  glPopMatrix
  
  glFlush
  glutSwapBuffers
)

Screen timer := method(v,
  timerCount = timerCount + 1
  if (timerCount > divisions, timerCount = 0)
  glutTimerFunc(1/divisions, 0)
  self display
)

Screen run := method(
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(512, 512)
  glutInit
  glutCreateWindow("")
  glutEventTarget(Screen)
  glutDisplayFunc
  glutReshapeFunc
  glutTimerFunc(100, 0)
  glEnable(GL_BLEND)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
  glutMainLoop
)

Screen run



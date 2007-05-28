#!/usr/bin/env io

debugCollector := 1

// a simple OpenGL program
// Steve Dekorte, 2002

Screen := Object clone
Screen appendProto(OpenGL)
Screen angle := 0
Screen t := 0
Screen v := 1
Screen dlist := nil
Screen reshape := method(w, h, 
  self width := w
  self height := h
  glViewport(0,0,w,h)
  gluOrtho2D(0,0,w,h)
  glClear(GL_COLOR_BUFFER_BIT)
  glutPostRedisplay
)

Screen setup := method(
  if (dlist == nil,
    self dlist := glGenLists(1) 
    glNewList(dlist, GL_COMPILE)
    self draw
    glEndList
  )
)

Screen display := method(
  glClearColor(1, 1, 1, 1)
  glClear(GL_COLOR_BUFFER_BIT)
  self setup
  gluOrtho2D(0,0,width,height)

  glPushMatrix
  glColor4d(.6, .6, .6, 1)
  glTranslated(-1, -.98, 0)
  glScaled(.001, .001, 0)
  glutStrokeString(0, "Io OpenGL lines demo")
  glPopMatrix
  
  glPushMatrix
  glRotated(angle, 0, 0, 1)
  glScaled(1.4, 1.4, 0)
  glScaled(v, v, 0)
  for(i, 1, 10,
    glCallList(dlist)
    glScaled(.7, .7, 0)
    glRotated(angle, 0, 0, 1)
  )
  glPopMatrix
    
  glFlush
  glutSwapBuffers
)

Screen draw := method(
  glColor4d(.6, .6, .6, 1)
  glColor4d(0, 0, 0, 1)
  glBegin(GL_LINE_LOOP)
  glVertex3d(-.5, -.5, 0)
  glVertex3d( .5, -.5, 0)
  glVertex3d( .5,  .5, 0)
  glVertex3d(-.5,  .5, 0)
  glEnd
)

Screen timer := method(v,
  angle = angle + .03
  glutTimerFunc(1, 0)
  self display
)

Screen run := method(
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize(512, 512)
  glutInit
  glutCreateWindow("lines")
  glutEventTarget(Screen)
  glutDisplayFunc
  glutReshapeFunc
  glutTimerFunc(100, 0)
  
  // turn on antialiasing
  glEnable(GL_LINE_SMOOTH)
  glEnable(GL_BLEND)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
  glLineWidth(1)
  glutMainLoop
)

Screen run



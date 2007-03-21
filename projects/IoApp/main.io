// A program to test OpenGL events
// Steve Dekorte, 2002

debugCollector := 1

Screen := Object clone
Screen parent := OpenGL
Screen timerCount := 0
Screen y := 0
Screen x := 0
Screen xDown := nil
Screen yDown := nil

/*
if (Lobby ?Font,
  Screen font := Font clone open("Library/Fonts/Free/Sans/Normal.ttf") setPixelSize(14)
)
*/
Screen action := "move mouse to start"

Screen reshape := method(w, h,
  write("Screen reshape(", w, ", ", h, ")\n")
  self width := w
  self height := h
  glViewport(0,0,w,h)
  glMatrixMode(GL_PROJECTION)
  glLoadIdentity
  gluOrtho2D(0, w, 0, h)
  glMatrixMode(GL_MODELVIEW)
  glLoadIdentity
  glClearColor(1, 1, 1, 1)
  display
)

Screen display := method(
  //write("Screen display\n")
  glClear(GL_COLOR_BUFFER_BIT)
  glLoadIdentity

  if (xDown, self drawSelection)
  self drawCrossHairs
  self drawActionMessage
  self drawTimerMessage

  glFlush
  glutSwapBuffers
)

Screen drawCrossHairs := method(
  glPushMatrix
  glColor4d(.7, .7, .7, 1)
  glBegin(GL_LINES)
  glVertex3d(x, 0, 0)
  glVertex3d(x, height, 0)
  glVertex3d(0, y, 0)
  glVertex3d(width, y, 0)
  glEnd
  glPopMatrix
)

Screen drawSelection := method(
  glColor4d(.7, .7, .7, .1)
  glBegin(GL_POLYGON)
  glVertex3d(xDown, yDown, 0)
  glVertex3d(xDown, y, 0)
  glVertex3d(x, y, 0)
  glVertex3d(x, yDown, 0)
  glEnd

  glColor4d(0,0,0, .1)
  glBegin(GL_LINE_LOOP)
  glVertex3d(xDown, yDown, 0)
  glVertex3d(xDown, y, 0)
  glVertex3d(x, y, 0)
  glVertex3d(x, yDown, 0)
  glEnd

  b := 5
  glColor4d(1, 0, 0, 1)
  glRectd(x-b, y-b, x+b, y+b)
)

Screen drawActionMessage := method(
  glColor4d(.5, .5, .5, 1)
  glColor4d(0,0,0, 1)
  glPushMatrix
  glTranslated(10, 10, 0)
  drawString(action)
  glPopMatrix
)

Screen drawString := method(string,
  if (self ?font) then (
    self font drawString(string)
  ) else (
    glPushMatrix
    glScaled(.15, .15, 0)
    glutStrokeString(0, string)
    glPopMatrix
  )
)

Screen drawTimerMessage := method(
  glColor4d(.5, .5, .5, 1)
  glColor4d(0,0,0, 1)
  glPushMatrix
  glTranslated(10, 30, 0)
  self drawString("timer ".. timerCount)
  glPopMatrix
)


Screen keyboard := method(key, x, y,
  self action := "keyboard(" .. key asCharacter ..", " .. x asString(0, 0) .. ", ".. y asString(0, 0) .. ")"
  self x := x; self y := height - y
  self display
)

Screen motion := method(x, y,
  self action := "motion(" .. x asString(0, 0) .. ", ".. y asString(0, 0) .. ")"
  self x := x; self y := height - y
  self display
)

Screen passiveMotion := method(x, y,
  self action := "passiveMotion(" .. x asString(0, 0) .. ", " .. y asString(0, 0) .. ")"
  self x := x; self y := height - y
  self display
)

Screen mouse := method(button, state, x, y,
  self action := "mouse(" .. button asString(0, 0) ..", " .. state asString(0, 0) .. ", " .. x asString(0, 0) .. ", " .. y asString(0, 0) .. ")"
  self x := x; self y := height - y
  if (state == 0, 
    self xDown := self x; self yDown := self y, 
    self xDown := nil; self yDown := nil
  )
  self display
)

Screen timer := method(v,
    //write("Screen timer ", timerCount, "\n")
    timerCount = timerCount + 1
    glutTimerFunc(100, 0)
    self display
)

Screen run := method(
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA )
    glutInitWindowSize(512, 512)
    glutInit
    glutCreateWindow("Events Test")
    glutEventTarget(Screen)
    glutDisplayFunc
    glutKeyboardFunc
    glutMotionFunc
    glutMouseFunc
    glutReshapeFunc
    glutPassiveMotionFunc
    glutTimerFunc(100, 0)

    glEnable(GL_LINE_SMOOTH)
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE)
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
    glLineWidth(1)
    glutMainLoop
)

Screen acceptsDrop := method(x, y, dropType, data,
    writeln("Io acceptsDrop: ", x, ",", y, " ", dropType, " : ", data)
    if (x < 100, return 1)
    return 0
)

Screen drop := method(x, y, dropType, data,
    writeln("Io got drop: ", x, ",", y, " ", dropType, " : ", data)
)

Screen run



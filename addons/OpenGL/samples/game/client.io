#!/usr/bin/env io


doFile(Path with(System launchPath, "Tank.io"))

systemfont := Font clone open("Library/Fonts/Free/FreeSans.ttf") setPointSize(14)

GameClient := Object clone
GameClient parent := OpenGL
GameClient objects := Map clone
GameClient player := nil
GameClient clientId := "nil"
GameClient message := "running client"

GameClient reshape := method(w, h, 
  self width := w
  self height := h
  glViewport(0, 0, w, h)
  glMatrixMode(GL_PROJECTION)
  glLoadIdentity
  gluOrtho2D(0, w, 0, h)
  glMatrixMode(GL_MODELVIEW)
  glLoadIdentity
  glutPostRedisplay
)

GameClient display := method(
  glClearColor(.1,.6,.1,1)
  glClearColor(0,0,0,1)
  glClear(GL_COLOR_BUFFER_BIT)
  glLoadIdentity
  glPushMatrix
  objects foreach(i, object, object draw)
  
  glColor4d(1,1,1,1)
  glTranslated(10,10,0)
  systemfont drawString(message)
  glPopMatrix
  glFlush
  glutSwapBuffers
)

GameClient keyboard := method(key, x, y,
  k := key asCharacter
  self message := "key := " .. k
  self serverConnection send("k", k)
)

GameClient motion := method(x, y, nil)
GameClient passiveMotion := method(x, y, nil)
GameClient mouse := method(button, state, x, y, nil)

GameClient timer := method(v,
  glutTimerFunc(10, 0)
  yield
  self display
)

GameClient run := method(
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA )
  glutInitWindowSize(512, 512)
  glutInit
  glutCreateWindow("Multiplayer Game")
  glutEventTarget(GameClient)
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
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
  glLineWidth(1)
  
  GameClient connect
  
  glutMainLoop
)

// --- Networking -----------------------------------

GameClient connect := method(
    self serverConnection := ServerConnection setDelegate(self) 
    self serverConnection setup
    self serverConnection @run
)

// --- Delegate methods -----------------------------------

GameClient setClientId := method(id,
  write("i ", id, "\n")
  self clientId := id
)

GameClient add := method(id, type,
  write("add ", id, " '", type, "'\n")
  objects atPut(id, getSlot(type) clone)
)

GameClient remove := method(id,
  write("remove ", id, "\n")
  objects remove(id)
)

GameClient update := method(id, x, y, angle,
  write("update ", id, "\n")
  obj := objects at(id)
  if (obj) then(
    obj position x <- x
    obj position y <- y
    obj angle <- angle
  ) else (
    write("no object with id '", id, "' found for update\n")
  )
)

// --- ServerConnection -----------------------------------

ServerConnection := Object clone
ServerConnection setDelegate := method(d, self delegate := d; self)
ServerConnection setup := method(
  ip := if(System args at(0), System args at(0), "67.118.104.220")
  port := if(System args at(1), System args at(1), 8888)
  self socket := Socket clone setHost(ip) setPort(port) connect 
  if (socket == nil, write("Unable to connect to ", ip, " on port ", port, "\n"); exit)
)

ServerConnection run := method(
  while (socket isOpen,
    if(socket read, 
      text := socket readBuffer asString
      //Object write("got: " .. text .. "\n")
      p := text splitNoEmpties(" ")
      action := p at(0)
      if (action == "i", delegate setClientId(p at(1)) )
      if (action == "a", delegate append(p at(1), p at(2)) )
      if (action == "u", 
        delegate update(p at(1), p at(2) asNumber, p at(3) asNumber, p at(4) asNumber) 
      )
      if (action == "r", delegate remove(p at(1)) )
    )
    socket readBuffer empty
  )
  exit
)

ServerConnection send := method(action, value,
  s := action .. " " .. value .. " "
  write("send: " , s, "\n")
  socket write(s)
)

GameClient run





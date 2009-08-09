#!/usr/bin/env io

// an AudioPad inspired demo
// Steve Dekorte, 2003

debugCollector := 1
Scheduler setTimers(0.001)

AudioDevice open

/*
d := Object clone
d total := 0
d buf := Buffer clone sizeTo(5000000)
d encoder := MP3Encoder clone
d write := method(b,
  //encoder encode(b) 
  total += b size
  buf appendSeq(b)
  write(total, " raw, ", encoder outBuffer size, " encoded\n")
  AudioDevice write(b)
  if (buf size > 4000000, 
	s := Sound clone
	s setSampleRate(44100)
	s setChannels(2)
	s setBuffer(buf)
	s setPath("mixtest.mp3")
	s write
	exit
  )
)
AudioMixer setAudioDevice(d)
*/
/*
s1 := Sound clone open(Path with(System launchPath, "SampleLoops/drum.wav"))
s2 := Sound clone open(Path with(System launchPath, "SampleLoops/bass.wav"))
s3 := Sound clone open(Path with(System launchPath, "SampleLoops/synth.wav"))
AudioMixer addSound(s1)
AudioMixer addSound(s2)
AudioMixer addSound(s3)
wait(10)
exit
*/

jitter := List clone append(
     Point clone set(.375, .25),
     Point clone set(.125, .75),
     Point clone set(.875, .25),
     Point clone set(.625, .75)
)

jitterDraw := method(
  //glColor4d(1, 1, 0, .5/(jitter size))
  m := thisMessage argAt(0)
  jitter foreach(j, 
    glPushMatrix 
    j glTranslate
    m doInContext(sender)
    glPopMatrix
  )
)

doFile(Path with(System launchPath, "Thing.io"))

Mix := Object clone
Mix appendProto(OpenGL)
Mix things := List clone 
Mix selectedThings := List clone 
Mix mpos := Point clone
Mix dpos := Point clone
Mix lastMouseDownTime := Date clone now
Mix fullScreen := nil
Mix framesPerSecond := 40

sansFont12 := Font clone open(Path with(System launchPath, "../../IoResources/Library/Fonts/Free/Sans/Bold.ttf")) setPixelSize(17)
sansFont24 := Font clone open(Path with(System launchPath, "../../IoResources/Library/Fonts/Free/Sans/Bold.ttf")) setPixelSize(38)

Mix reshape := method(w, h, 
  self width := w
  self height := h
  glViewport(0, 0, w, h)
  glMatrixMode(GL_PROJECTION)
  glLoadIdentity
  gluOrtho2D(0, w, 0, h)
  glMatrixMode(GL_MODELVIEW)
  glLoadIdentity
  glClearColor(0, 0, 0, 1)
  display
)

Mix display := method(
  glClear(GL_COLOR_BUFFER_BIT)
  glLoadIdentity
  things foreach(thing, thing display)
  glFlush
  glutSwapBuffers
)

Mix keyboard := method(key, x, y, Nop)

Mix updateSpeakers := method(
  things foreach(t, 
    t ?updateLeftSpeaker(leftSpeaker)
    t ?updateRightSpeaker(rightSpeaker)
  )
)

Mix motion := method(x, y,
  y := height - y
  dpos set(x, y) -= mpos

  selectedThings foreach(t, 
    //write("moving ", t uniqueId, "\n")
    t pos += dpos
  )
  self updateSpeakers
  mpos set(x, y)
  self display
)

Mix mouse := method(button, state, x, y,
  y := height - y
  dpos set(x, y)
  //write("mouse ", x, " ", y, "\n")
  selectedThings foreach(t, t unselect)
  selectedThings empty
  things foreach(t, 
    if(t touchesPoint(dpos), 
      selectedThings append(t)
      t select
    )
  )
  mpos set(x, y)
  self display
)

Mix timer := method(v,
  if (v == 0) then(
    glutTimerFunc(1000/200, 0)
    yield
  ) else (
    things foreach(t, t ?timer)
    glutTimerFunc(1000/framesPerSecond, 1)
    self display
  )
)


Mix run := method(
  self width := 800
  self height := 800
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA)
  glutInitWindowSize(width, height)
  
  Mix leftSpeaker  pos set(width/2 - width/15, height*.8)
  Mix rightSpeaker pos set(width/2 + width/15, height*.8)

  glutInit
  glutCreateWindow("Mix")
  glutEventTarget(Mix)
  glutDisplayFunc
  glutKeyboardFunc
  glutMotionFunc
  glutMouseFunc
  glutReshapeFunc
  //glutPassiveMotionFunc
  glutTimerFunc(0, 0)
  glutTimerFunc(0, 1)
  
  //glEnable(GL_LINE_SMOOTH)
  //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
  //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST)
  //glLineWidth(1)
  glEnable(GL_BLEND)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
  self updateSpeakers
  AudioMixer setSamplesPerBuffer(44100/10)
  AudioMixer @@start
  glutMainLoop
)

lastLoop := nil
Mix addLoopAt := method(path, x, y, 
  loop := Loop clone open(path)
  loop pos set(x, y) 

  if (lastLoop, loop sizeToIntegerMultipleOfLoop(lastLoop))
  lastLoop := loop
  loop @start
  Mix things append(loop)
  return loop
)

Mix leftSpeaker  := Speaker clone setName("L")
Mix rightSpeaker := Speaker clone setName("R")
Mix things append(Mix leftSpeaker, Mix rightSpeaker)

Mix addLoopAt(Path with(System launchPath, "Loops/drum.wav"), 200, 75)
Mix addLoopAt(Path with(System launchPath, "Loops/bass.wav"), 400, 250)
Mix addLoopAt(Path with(System launchPath, "Loops/synth.wav"), 600, 75)

/*
Mix addLoopAt(Path with(System launchPath, "Loops/Progeny/keyboards.wav"), 400, 250)
Mix addLoopAt(Path with(System launchPath, "Loops/Progeny/guitars.wav"), 600, 75)
Mix addLoopAt(Path with(System launchPath, "Loops/Progeny/beltram.wav"), 700, 75)
Mix addLoopAt(Path with(System launchPath, "Loops/Progeny/trumpets.wav"), 100, 75)
Mix addLoopAt(Path with(System launchPath, "Loops/Progeny/bass.wav"), 200, 75)
Mix addLoopAt(Path with(System launchPath, "Loops/Progeny/MAM Harmony.wav"), 300, 75)
Mix addLoopAt(Path with(System launchPath, "Loops/Progeny/oz.wav"), 350, 175)
*/

Mix run




Sound pixelsPerSample := method(
   pixelsPerVolume /  samplesPerVolume
)

Point darken := method(v, 
  if (v == nil, v = .5)
  setRed(red * v)
  setGreen(green * v)
  setBlue(blue * v)
  self
)

WaveView := View clone do(
    protoName := "WaveView"
    position set(100, 100)
    size set(Screen width, 48)
    barColor := Color clone set(.4,.4,.4,1)
    resizeWidth  = 110
    resizeHeight = 101

    sampleOffset := 0

    maskColor := Color clone set(0,0,.5,1)
    waveColor := Color clone set(1,0,0,1)
    newSlot("delegate")
    
    init := method(
	resend
	self sound := nil
	self waveForm := nil
	self volumeMask := nil
    )
    
    setVolume := method(v,
	waveColor alpha = v
	needsRedraw = 1
    )

    setSound := method(s,
	waveForm = nil
	volumeMask = nil
	oldSound := sound
	
	sound = s
	path := sound path asLowercase
	b := Color clone set(.6,.6,.6,1)
	if (path contains("guitar"),  b set(0.4, 0.9, 0.0, 1))
	if (path contains("bass"),    b set(0.0, 0.0, 1.0, 1))
	if (path contains("key"),     b set(0.0, 0.6, 1.0, 1))
	if (path contains("synth"),   b set(0.8, 0.4, 0.0, 1))
	if (path contains("drum"),    b set(0.8, 0.4, 0.0, 1))
	if (path contains("fx"),      b set(0.4, 0.4, 0.4, 1))
	if (path contains("orch"),    b set(0.0, 0.8, 0.4, 1))
	if (path contains("horn"),    b set(0.8, 0.8, 0.0, 1))
	//backgroundColor = Color clone set(1,1,1,1)
	//bright := method(v, v+((1-v)*.9))
	w := b clone darken
	
	waveColor = w
	backgroundColor = b
	sound setVolumeFromTo(0, 0, Sound defaultPlayLengthInBars * Sound beatsPerBar)
	setPlayLengthInBars(Sound defaultPlayLengthInBars)
	if (oldSound, sound copyVolumesFrom(oldSound))
	if (oldSound, sound setPlayPosition(oldSound playPosition))
	needsRedraw = 1

	self testSound := sound clone
	testSound setVolumeFromTo(255, 0, 100)
	testSound loopingOn
	testSound setPlayLengthInBars(8)
    )

    setPlayLengthInBars := method(bars,
	sound setPlayLengthInBars(bars)
	sound setPixelsPerVolume(Sound pixelsPerBeat)
	waveForm = nil
	volumeMask = nil
	setWidth(Sound pixelsPerBeat * bars * Sound beatsPerBar)
    )

    drawBackground2 := method(
      w := width
      h := height

      c1 := 1
      c := .85
      sh := h*.7 

      b := backgroundColor clone darken(.87)
      // draw bottom middle shading
      glBegin(GL_QUADS)
	b glColor
	glVertex3i(0, 0, 0) 
	backgroundColor glColor
	glVertex3i(0, sh, 0)
	glVertex3i(w, sh, 0)
	b glColor
	glVertex3i(w, 0, 0)
      glEnd

      // draw top middle 
      backgroundColor glColor
      glRecti(0, sh, w, h)
    )

    draw := method(  
	//write("WaveView draw\n")
	if (waveForm == nil, 
	    sound updateWaveFormDisplayList
	    waveForm = sound waveFormDisplayList

	    sound updateVolumeMaskDisplayList
	    volumeMask = sound volumeMaskDisplayList
	)

	doWithinDisplayList(
	    drawBackground2
	    drawWaveForm
	    drawVolumeMask
	    drawBars
	    //drawLoopDividers
	    //drawOutline
	)
	drawPlayPosition
    )

    drawOutline := nil

    drawWaveLine := method(
      waveColor glColor
      glPushMatrix
      glTranslated(0, height/2, 0)
      glBegin(GL_LINES)
      glVertex2i(0, 0)
      glVertex2i(width, 0)
      glEnd
      glPopMatrix
    )
    
    
    drawWaveForm := method(
      glPushAttrib(GL_LINE_BIT)
      glEnable(GL_LINE_SMOOTH)
      glLineWidth(1)
      waveColor glColor

      w := widthPerLoop
      max := 2 * sound playLength * sound pixelsPerSample
      x := 0
      dx := w
      while(x < max,
	glPushMatrix
	glTranslatei(x, height/2, 0)
	glScaled(1, height*2, 1)
	glCallList(waveForm)
	glPopMatrix
	x = x + dx
      )
      glPopAttrib
    )
    
    widthPerLoop := method(
	//write("sound samplesPerVolume  = ", sound samplesPerVolume , "\n")
	//write("sound pixelsPerSample  = ", sound pixelsPerSample , "\n")
	//write("sound pixelsPerVolume  = ", sound pixelsPerVolume , "\n")
	//write("TrackMeter pixelsPerBeat  = ", TrackMeter pixelsPerBeat , "\n")
	
        volumes := sound sampleCount / sound samplesPerVolume
	sound pixelsPerVolume * volumes // sound channels
    )
    
    drawVolumeMask := method(
	maskColor glColor
	glPushMatrix
	glScaled(1, height, 1)
	glCallList(volumeMask)
	glPopMatrix
    )
    

    drawLoopDividers := method(
	w := widthPerLoop 
	x := 0
	glPushAttrib(GL_LINE_BIT)
	glLineWidth(2)
	glColor4d(.6,.6,.6, .4)
	while(x < width,
	    glBegin(GL_LINES)
	    glVertex2i(x, 0)
	    glVertex2i(x, height)
	    glEnd
	    x = x + w
	)
	glPopAttrib
    )
    
    drawPlayPosition := method(
      if (sound playPosition > 0,
	  glColor4d(0,0,0,.8)
	  glLineWidth(2)
	  glBegin(GL_LINES)
	  x := sound pixelsPerSample * sound visiblePlayPosition 
	  glVertex2i(x, 0)
	  glVertex2i(x, height)
	  glEnd
	  glLineWidth(1)
      )
    )

    barList := nil
    
    drawBars := method(
	glPushAttrib(GL_LINE_BIT)
	glDisable(GL_LINE_SMOOTH)
	barColor glColor
	x := 0
	bar := 0

	glBegin(GL_LINES)
	while (x < width,
	    if (sound volumeAt(bar) == 0,
		lw := if(bar % 4 == 0, 2, 1)
		glLineWidth(lw)
		glVertex2i(x, height)
		glVertex2i(x, 0)
		if (lw == 2,
		  glVertex2i(x+1, height)
		  glVertex2i(x+1, 0)		
		)
	    )
	    bar = bar + 1
	    x = bar * sound pixelsPerVolume
	)
	glEnd 
	glPopAttrib
    )

    autoDrawMode := 1
    
    leftMouseDown := method(
	self startDragX := viewMousePoint x
	s1 := startDragX / sound pixelsPerVolume
	if (autoDrawMode, drawMode = sound volumeAt(s1) toggle)
	if (delegate, delegate ?waveViewLeftMouseDown(self))
	motion
    )

    leftMouseUp := method(
	self startDragX := nil
	if (delegate, delegate ?waveViewLeftMouseUp(self))
    )

    leftMouseMotion := method(
	if (?(startDragX) == nil, self startDragX := viewMousePoint x)
	pixelsPerVolume := sound pixelsPerVolume
	s1 := startDragX / pixelsPerVolume
	s2 := viewMousePoint x / pixelsPerVolume
	s1 = (s1 - .5) roundDown
	s2 = (s2 - .5) roundDown
	sound setVolumeFromTo(if(drawMode==1, 255, 0), s1, s2)
	sound updateVolumeMaskDisplayList
	needsRedraw = 1
	yield
	glutPostRedisplay
    )

    drawMode := 1
    setDrawMode := method(m, drawMode = m)
    drawModeOn := method(drawMode = 1; autoDrawMode = nil)
    eraseModeOn := method(drawMode = 0; autoDrawMode = nil)

)


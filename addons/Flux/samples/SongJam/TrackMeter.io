
TrackMeter := FlatButton clone do(
    protoName := "TrackMeter"
    position set(100, 100)
    size set(Screen width, 64)

    textColor := Color clone set(.4, .4, .4, 1)
    fillImage  := ImageManager item("Window/TitleBar/Middle")
    font := systemFont

    isTrackMeter := 1
    resizeWidth := 110
    resizeHeight = 101

    bars := 10
    beats := method(bars * Sound beatsPerBar)
    
    setPlayLengthInBars := method(b,
	bars = b 
	setWidth(Sound pixelsPerBeat * beats)
    )

    draw := method(doWithinDisplayList(draw2))
    
    draw2 := method(
	fillImage drawTexture(width, fillImage height)

	glLineWidth(2)
	textColor glColor

	for (bar, 0, bars,
	    x := (Sound pixelsPerBeat * bar * Sound beatsPerBar) + 1

	    glBegin(GL_LINES)
	    glVertex2i(x, 1)
	    glVertex2i(x, 5)
	    glEnd 

	    title := (bar + 1) asString
	    glPushMatrix  
	    glTranslated(x + 3, 7, 0)	       
	    font drawString(title)
	    glPopMatrix
	)
    )
    
    drawOutline := nil
    
    leftMouseDown := method(
	x := viewMousePoint x
	volumePos := x / Sound pixelsPerBeat
	samplePos := volumePos * Sound samplesPerVolume * 2
	self selectedVolumePosition := samplePos 
	doAction
    )
    
    leftMouseDoubleClickDown := method(
	leftMouseDown
	actionTarget playButton do(leftMouseDown; leftMouseUp)
    
    )

    leftMouseMotion := nil
)


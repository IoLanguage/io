
VideoView := View clone do(
	position setX(0) setY(0)
	size setWidth(640) setHeight(480)
	newSlot("image")
	newSlot("video")
    isRunning ::= false
    
	init := method(
		resend
		AVCodec
		video = Video clone
		image = video image
	)

	sizeToImage := method(
		if(image, size setWidth(image width) setHeight(image height))
		self
	)
	
	setIsClipped(false)
	
	draw := method(
		if(image, drawProportional)
	)
	
	translateToPlacement := method(
        wr := width  / image originalWidth
		hr := height / image originalHeight
		if (wr < hr) then(
		    self tx := 0
		    self ty := (height - image originalHeight * wr)/2
		    self tr := wr
		) else(
            self tx := (width - image originalWidth * hr) / 2
		    self ty := 0
		    self tr := hr
		)
        glTranslated(tx, ty, 0)
        glScaled(tr, tr, 1)
	)
	
	drawProportional := method(
		glPushMatrix
		translateToPlacement
		glColor4d(1, 1, 1, 1)
		image drawTexture
		glPopMatrix
		self
	)
	
	newSlot("isSelected", false)
	select := method(setIsSelected(true))
	unselect := method(setIsSelected(false))
	acceptsFirstResponder := false
	
	open := method(path,
	    //writeln("videoView open")
        video setPath(path)
        video open
        video readNextFrame
        play
	)

    play := method(
        setIsRunning(true)
        topWindow addTimerTargetWithDelay(self, video framePeriod)
    )
    
    stop := method(setIsRunning(false))

    timer := method(n,
        //writeln("VideoView timer")
        isRunning ifFalse(return)
        topWindow addTimerTargetWithDelay(self, video framePeriod)
        video readNextFrame
        if(video isDone, video start)
        glutPostRedisplay
    )
)

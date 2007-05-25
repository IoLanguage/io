
VideoView := View clone do(
	position setX(0) setY(0)
	size setWidth(640) setHeight(480)
	newSlot("image")
	newSlot("videoDecoder")

	init := method(
		resend
		AVCodec
		videoDecoder = Video clone
		image = videoDecoder image
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
		    self ty := (height - (image originalHeight * wr))/2
		    self tr := wr
		) else(
            self tx := (width - (image originalWidth * hr)) / 2
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
        videoDecoder setFileName(path)
        videoDecoder open
        videoDecoder readNextFrame
        play
	)

    play := method(
            topWindow addTimerTargetWithDelay(self, videoDecoder framePeriod)
    )

    timer := method(n,
        //writeln("VideoView timer")
        topWindow addTimerTargetWithDelay(self, videoDecoder framePeriod)
        videoDecoder readNextFrame
        if(videoDecoder isDone, videoDecoder start)
        glutPostRedisplay
    )
)

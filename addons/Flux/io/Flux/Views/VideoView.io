
VideoView := View clone do(
	position setX(0) setY(0)
	size setWidth(640) setHeight(480)
	image ::= nil
	videoDecoder ::= nil

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
        wr := width  / image width
		hr := height / image height
		if (wr < hr) then(
		    self tx := 0
		    self ty := (height - image height * wr)/2
		    self tr := wr
		) else(
            self tx := (width - image width * hr) / 2
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
	
	isSelected ::= false
	select := method(setIsSelected(true))
	unselect := method(setIsSelected(false))
	acceptsFirstResponder := false
	
	open := method(path,
	    //writeln("videoView open")
        videoDecoder setPath(path)
        videoDecoder open
        videoDecoder readNextFrame
        play
	)

    isPlaying ::= false
    repeatOn ::= true
    
    play := method(
        writeln("VideoView play")
        setIsPlaying(true)
        topWindow addTimerTargetWithDelay(self, videoDecoder framePeriod)
    )
    
    stop := method(setIsPlaying(false))

    timer := method(n,
        writeln("VideoView timer")
        if(isPlaying,
            topWindow addTimerTargetWithDelay(self, videoDecoder framePeriod)
            videoDecoder readNextFrame
            if(videoDecoder isDone and repeatOn, videoDecoder start)
            glutPostRedisplay
        )
    )
)

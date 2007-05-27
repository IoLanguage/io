VideoDecoder := AVCodec clone do(
	newSlot("internalCoro")
	newSlot("externalCoro")
	newSlot("frameNumber", 0)
	newSlot("sampleRateConverter", nil)
	newSlot("audioOn", false)
	isDone ::= false
	
	init := method(
	   self image := Image clone
	)
	
    willProcess := getSlot("yield")
    
    _open := getSlot("open")
    open := method(
        _open
        writeln("audioSampleRate = ", audioSampleRate)
        writeln("audioChannels = ", audioChannels)
        audioOn := (audioSampleRate == 44100 and audioChannels == 2)
        writeln("audioOn = ", audioOn)
        /*

        if(?audioSampleRate and audioSampleRate != 44100,
            writeln("audioSampleRate setup")
            self sampleRateConverter := SampleRateConverter clone
            sampleRateConverter setOutputToInputRatio(44100 / audioSampleRate)
            //sampleRateConverter setOutputToInputRatio(1.5)
            writeln("ratio = ", sampleRateConverter outputToInputRatio)
            //sampleRateConverter outputBuffer setItemType("float32")
            //audioOutputBuffer setItemType("float32")
        )
        */
    )
    
    didProcess := method(
	    if(isDone, return nil)
        setInternalCoro(Coroutine currentCoroutine)
        
        /*
        if(audioOn,
        if(sampleRateConverter, 
            //writeln("audioSampleRate converting")
            if(audioChannels == 1, audioOutputBuffer duplicateIndexes)
            sampleRateConverter inputBuffer appendSeq(audioOutputBuffer)
            audioOutputBuffer empty 
            sampleRateConverter process
            AudioDevice asyncWrite(sampleRateConverter outputBuffer)
            sampleRateConverter outputBuffer empty 
        ,
            //if(audioChannels == 1, audioOutputBuffer duplicateIndexes)
            AudioDevice asyncWrite(audioOutputBuffer)
            audioOutputBuffer empty                   
        )
        )
        */
        
        AudioDevice asyncWrite(audioOutputBuffer)
        audioOutputBuffer empty  
            
        if(frames size > 0,
            if(externalCoro, externalCoro resume)
        )
	)
	
	readNextFrame := method(
	    if(isDone, return nil)
	    while(internalCoro == nil, yield)
        setExternalCoro(Coroutine currentCoroutine)
        internalCoro resume
        frameNumber = frameNumber + 1
        frame := frames removeFirst
        if(frame, image setDataWidthHeightComponentCount(frame, videoSize x, videoSize y, 3), nil)
	)
	
	_startDecoding := getSlot("startDecoding")
	
	startDecoding := method(
	   setIsDone(false)
	   _startDecoding
	   setIsDone(true)
	   externalCoro resume
    )
    
	start := method(
	   self @@startDecoding
	   if(internalCoro, internalCoro resume)
    )
)

VideoViewer := Object clone do(
	appendProto(OpenGL)

	init := method(
		self videoDecoder := VideoDecoder clone setFileName(System args at(1))
		self image := videoDecoder image
	)
	init
		
	reshape := method(w, h, 
		self width := w
		self height := h
		glViewport(0, 0, w, h)
		glLoadIdentity
		gluOrtho2D(0, w, 0, h)
		glutPostRedisplay
	)
	
	display := method(
		glClearColor(0,0,0, 1)
		glClear(GL_COLOR_BUFFER_BIT)
		glPushMatrix
        draw
		glPopMatrix
		glutSwapBuffers
	)
	
	draw := method(
	    if(image originalWidth == 0, return)

		wr := width / image originalWidth
		hr := height / image originalHeight
		
		if (wr < hr) then(
			glTranslated(0, (height - image originalHeight * wr)/2, 0)
			glScaled(wr, wr, 1)
		) else(
			glTranslated((width - image originalWidth * hr) / 2, 0, 0)
			glScaled(hr, hr, 1)
		)
        
		image drawTexture	
	)
        
    timer := method(n,
        if(n == 0, 
            //writeln("starting")
            glutTimerFunc(0, 1)
            videoDecoder open
            videoDecoder start
            videoDecoder readNextFrame
            glutReshapeWindow(image originalWidth, image originalHeight)
            ,
            glutTimerFunc(videoDecoder framePeriod * 1000, 1)
            videoDecoder readNextFrame
    		if(videoDecoder isDone, videoDecoder start)
        )

        //videoDecoder println
        glutPostRedisplay
    )
    
	run := method(
	    AudioDevice open
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE)
		glutInitWindowSize(512, 256)
		glutInit
		glutCreateWindow("")
		glutEventTarget(self)
		glutDisplayFunc
		glutReshapeFunc
		glutTimerFunc(0, 0)		
		glutMainLoop
	)
)

VideoViewer run



OSWindow := Responder clone do(	    
 	frame ::= Box clone do(size set(500, 500))
 	
	position := method(frame origin)
    setPosition := method(p, position copy(p); self)
    
    setSize := method(s, size copy(s); self)
    size := method(frame size)
    	
    width   := method(size width)
    height  := method(size height)
    
    title ::= ""
    firstResponder ::= nil
    backgroundColor ::= Color clone
    application  ::= nil
    subviews ::= List clone
    windowId ::= nil
    isFullScreen ::= false
	normalFrame ::= Box clone
    frameBeforeFullScreen ::= Box clone
   
    init := method(
		setFrame(frame clone)
		setFrameBeforeFullScreen(frameBeforeFullScreen clone)
		
		self subviews := List clone
		setFirstResponder(nil)
		
		self timers := Map clone
		
		self contentView := View clone setSize(size) resizeWithSuperview
        //contentView leftMouseDown := method(makeFirstResponder)
        self addSubview(contentView)
    )

    open := method(
		setWindowId(glutCreateWindow(title))
		//writeln("winId = ", self windowId)
		glutReshapeWindow(width, height)
		glutDisplayFunc
		glutKeyboardFunc
		glutSpecialFunc
		glutMotionFunc
		glutMouseFunc
		glutReshapeFunc
		enableAntialiasing
    )
    
    enableAntialiasing := method(  
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glLineWidth(1)
		glEnable(GL_LINE_SMOOTH)
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
		glShadeModel(GL_SMOOTH)
    )

    // --- Timer Events -------------

    addTimerTargetWithDelay := method(target, delay,
		id := target uniqueId 
		//writeln("addTimerTargetWithDelay(", target type, ", ", delay, ")")
		timers atPut(id asString, target)
		glutTimerFunc(delay*1000, id)
    )

    removeTimerWithTarget := method(target, 
		timers removeAt(target uniqueId asString)
    )

    Timer := Object clone do(
        target ::= nil
        action ::= nil
        delay  ::= 0
        window ::= nil
        run := method(window addTimerTargetWithDelay(self, delay))
        timer := method(target perform(action))
    )
    
    CountdownTimer := Object clone do(
        target ::= nil
        action ::= nil
        delay  ::= 0
        maxCount  ::= 10
        window ::= nil
        
        run := method(
            self count := maxCount
            window addTimerTargetWithDelay(self, delay)
        )
        
        timer := method(
            //writeln("CountdownTimer timer ", count)
            target perform(action, 1-count/maxCount)
            count = count - 1
            if(count != -1, window addTimerTargetWithDelay(self, delay)
)
        )
    )

    newCountdownTimer := method(
        CountdownTimer clone setWindow(self)
    )  
      
    newTimer := method(
        Timer clone setWindow(self)
    )
    
    timer := method(id,
		idString := id asString
		target := timers at(idString)
		//writeln("OSWindow timer id = ", id, " target = ", target type)
		if(target,
			timers removeAt(idString)
			if(target == self, yieldTimer, target timer(self))
		)
    )

	yieldTimerPeriod ::= .5
	
    yieldTimer := method(
		//writeln("OSWindow yieldTimer ", Coroutine yieldingCoros size)
		//writeln(".")
    	if(Coroutine yieldingCoros size > 0,
	    	yield
    		self addTimerTargetWithDelay(self, yieldTimerPeriod)
    	)	
    )
    
    // --- Subviews ---------------------------------
    
    addSubview := method(view, 
		subviews append(view)
		//view setSuperview(self)
		view setTopWindow(self)
    )
    
    removeSubview := method(view, 
		view releaseFirstResponder
		subviews remove(view)
		//view setSuperview(nil)
		//view setTopWindow(nil)
    )

    orderToFrontSubview := method(view,
		subviews remove(view)
		subviews append(view)
    )

    // --- Display ---------------------------------

    set3dPerpective := method(
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluPerspective(45, width/height, 0.1, 100)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity
    )

    set2dPerpective := method(
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluOrtho2D(0, width, 0, height)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity
    )

    
    reshapeToSize := method(size,
        glutSetWindow(self windowId)
		glutReshapeWindow(size x, size y)
    )
    
    reshape := method(w, h, 
		//write("reshape(", w, ",", h, ")\n")
		resizeTo(w, h)
		glViewport(0, 0, w, h)
		set2dPerpective
		glutPostRedisplay
    )

    resizeTo := method(w, h,
		w = w round
		h = h round
		dx := w - width
		dy := h - height 
		size set(w, h)
		subviews foreach(v, v resizeBy(dx, dy))
    )

    postRedisplay := getSlot("glutPostRedisplay")
    
    display := method(
		//glEnable(GL_TEXTURE_2D);
		glLoadIdentity
		glScissor(0, 0, width, height)
		drawBackground
		subviews foreach(v, v display)
		glFlush
		glutSwapBuffers
    )

    drawBackground := method(
		//glClearColor(20/256, 100/256, 184/256, 1)
		backgroundColor glClearColor
		//glClearColor(backgroundColor red, backgroundColor green, backgroundColor blue, 1)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    )
    
   // --- Events ---------------------------------

    setFirstResponder := method(fr, 
		firstResponder = fr
    )

    keyboard := method(key, x, y,
   		Keyboard key(key)
        if(firstResponder) then(
            firstResponder ?keyboard(key, x, y)
		) else(
            hv := hitView	
			if(hv, hv keyboard(key, x, y))
		)
		glutPostRedisplay
    )
    
    special := method(key, x, y, 
   		Keyboard special(key)
        if(firstResponder) then(
            firstResponder ?special(key, x, y)
		) else(
            hv := hitView	
			if(hv, hv special(key, x, y))
		)
		glutPostRedisplay
    )
	
    mouse := method(b, s, x, y,
        if(s == 1 and firstResponder, 
            firstResponder mouse
        ,
            hv := hitView	
            if(hv, hv mouse)
        )
		glutPostRedisplay
    )

    motion := method(x, y,
		if(firstResponder, firstResponder motion)
		//glutPostRedisplay
    )

    hitView := method(
		subviews reverseForeach(i, view, 
			v := view screenHit
			if(v, return v)
		)
		return nil
    )
    
    setIgnoreKeyboardRepeat := method(bool,
    	glutSetWindow(self windowId)
    	glutIgnoreKeyRepeat(if(bool, 1, 0))
    )
    
    getPosition := method(
    	frame origin set(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y))
    )
    
    fullScreenOff := method(
		//frame copy(frameBeforeFullScreen)
    	//glutReshapeWindow(size width, size height)
    	glutReshapeWindow(frameBeforeFullScreen size width, frameBeforeFullScreen size height)
    	//glutPositionWindow(position x, position y)
    	glutPositionWindow(frameBeforeFullScreen origin x, frameBeforeFullScreen origin y)
    	setIsFullScreen(false)
    )
 
	fullScreenOn := method(
		getPosition
		frameBeforeFullScreen copy(frame)
    	glutFullScreen
    	setIsFullScreen(true)
    )  
     
    toggleFullScreen := method(
    	if(isFullScreen, fullScreenOff, fullScreenOn)
    )

    setTitle := method(newTitle,
	title = newTitle
	if(windowId, glutSetWindowTitle(title))
    )
)


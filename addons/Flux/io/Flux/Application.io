

Application := Responder clone do(
    fonts := Object clone do(
		system := Object clone do(
			normal := FontManager item("Free/Sans/Normal") setPixelSize(15)
            bold   := FontManager item("Free/Sans/Bold") setPixelSize(15)
            mono   := FontManager item("Free/Mono/Bold") setPixelSize(15)
            term   := FontManager item("ProFont/Mono/Normal") setPixelSize(20)
            //big  := FontManager item("Free/Sans/Bold") clone setPixelSize(32)
		)
    )
    
    mainWindow ::= nil
    windows ::= List clone
    title ::= ""
    firstResponder ::= nil
	timers ::= Map clone
   
    setTitle := method(title,
    	mainWindow ?setTitle(title)
    )
    
    init := method(
		glutInit
		setWindows(List clone)
		setMainWindow(OSWindow clone)
    	addWindow(mainWindow)
		setTimers(Map clone)
    )

    // --- Windows -------------

    newWindow := method(
    	w := OSWindow clone
    	//writeln("App w = ", w uniqueId)
		addWindow(w)
		w
    )
    
    addWindow := method(w,
		w setApplication(self)
		windows append(w)
    )

    // --- run ---------------------------------
    
    run := method(
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH)
		glutInitWindowPosition(0, 0)
		glutInitWindowSize(100, 100)
		//glutInit
		glutEventTarget(self)
	    	//writeln("app glutTimerFunc")
		glutTimerFunc(0, 0)		
		mainWindow open
		glutMainLoop
    )
    
    appDidStart := method(
    	//writeln("appDidStart")
    	//m := OSMenu clone create
    	glutPostRedisplay
    )
    
    // --- Timer Events -------------

    /*
    addTimerTargetWithDelay := method(target, delay,
		mainWindow addTimerTargetWithDelay(target, delay)
    )

    removeTimerWithTarget := method(target, 
		mainWindow removeTimerWithTarget(target)
    )
    */
    
    screenHeight := method(
    	glutGet(GLUT_SCREEN_HEIGHT)
    )
    
	screenWidth := method(
    	glutGet(GLUT_SCREEN_WIDTH)
    )
   
   	currentWindow := method(
   		wid := glutGetWindow
   		windows detect(windowId == wid)
   	)
 
 	started ::= false
  	
    timer := method(id,
        //writeln("app timer")
        if(started == false, setStarted(true); self appDidStart)
        currentWindow timer(id)
    )
    
	reshape := method(w, h, 
    	//writeln("App reshape ", currentWindow id)
		currentWindow reshape(w, h)
    )

    display := method(
    	//writeln("App display ", currentWindow id)
		currentWindow display
    )

    keyboard := method(key, x, y,
    	w := currentWindow
 		y = w height - y
    	//writeln("App keyboard ", w id)
    	Keyboard key(key)
		w keyboard(key, x, y)
    )
    
    special := method(key, x, y, 
    	w := currentWindow
 		y = w height - y
    	//writeln("App special ", w id)
    	Keyboard special(key)
		w special(key, x, y)
    )
	
    mouse := method(b, s, x, y,
    	w := currentWindow
 		y = w height - y
		//writeln("App mouse ", w id)
		Mouse mouse(b, s, x, y)
		w mouse(b, s, x, y)
    )

    motion := method(x, y,
    	w := currentWindow
 		y = w height - y
		Mouse motion(x, y)
		w motion(x, y)
    )
)

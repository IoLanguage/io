

ResourceManager

Lobby systemFont := FontManager item("Free/Sans/Normal") setPixelSize(13)
Lobby systemFontBold := FontManager item("Free/Sans/Bold") setPixelSize(13)
Lobby systemFontBig := FontManager item("Free/Sans/Bold") clone setPixelSize(18)

//systemFontSmallBold := FontManager item("Free/FreeSansBold") setPixelSize(13)
//systemFontSmallBold2 := FontManager item("Free/FreeSansBold") setPixelSize(18)
//systemFont := Font clone open("/System/Library/Fonts/LucidaGrande.dfont") setPixelSize(13)
//systemFont2 := Font clone open("/System/Library/Fonts/LucidaGrande.dfont") setPixelSize(15)

Screen := GLObject clone
Object Screen := Screen

Screen do(
    type   := method("Screen" .. uniqueId)

    backgroundColor := Color clone
    
    firstResponder := nil

    // --- Timers -------------

    timers := Map clone

    addTimerTargetWithDelay := method(target, delay,
		id := target uniqueId 
		idString := id asString
		timers atPut(idString, target)
		glutTimerFunc(1000*delay, id)
    )

    removeTimerWithTarget := method(target, 
		timers removeAt(target uniqueId asString)
    )

    timer := method(id,
		idString := id asString
		target := timers at(idString)
		if(target,
			timers removeAt(idString)
			target timer
		)
    )

    // --- Open ---------------------------------

    position := Point clone set(500,500)
    size    := Point clone set(500,500)
    setSize := method(w, h, size set(w, h))
    width   := method(size width)
    height  := method(size height)

    title := ""
    setTitle := method(t, title = t)
    

    open := method(w, h,
		if (w and h, size set(w, h))
		glutInit
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)
		glutInitWindowPosition(position x, position y)
		glutInitWindowSize(width, height)
		glutCreateWindow(title)
		glutEventTarget(Screen)
		glutTimerFunc(1, 0)
		glutDisplayFunc
		glutKeyboardFunc
		glutSpecialFunc
		glutMotionFunc
		glutMouseFunc
		glutReshapeFunc
		enableAntialiasing
		glutMainLoop
    )

    // --- Subviews ---------------------------------

    subviews := List clone
    addSubview := method(view, subviews append(view))
    removeSubview := method(view, subviews remove(view))

    orderToFrontSubview := method(view,
		subviews remove(view)
		subviews append(view)
    )


    // --- Display ---------------------------------

    set3dPerspective := method(
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluPerspective(45, width/height, 0.1, 100)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity
    )

    set2dPerspective := method(
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluOrtho2D(0, width, 0, height)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity
    )

    reshape := method(w, h, 
		//write("reshape(", w, ",", h, ")\n")
		resizeTo(w, h)
		glViewport(0, 0, w, h)
		set2dPerspective
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

    display := method(
		//write("Screen display ", Date clone now asNumber, "\n")
		glLoadIdentity
		glScissor(0, 0, width, height)
		drawBackground
		subviews foreach(v, v display)
		glFlush
		glutSwapBuffers
    )

    drawBackground := method(
		//glClearColor(20/256, 100/256, 184/256, 1)
		glClearColor(backgroundColor red, backgroundColor green, backgroundColor blue, 1)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		//if (self ?xImage == nil, self xImage := ImageManager item("X"))
		//xImage draw
    )
    
    drawShadedBackground := method(
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		glBegin(GL_QUADS)
		glColor4d(0/256, 78/256, 184/256, 1)
		glVertex3d(0, height, 0) 
		glVertex3d(width, height, 0) 
		glColor4d(71/256, 147/256, 219/256, 1)
		glVertex3d(width, 0, 0) 
		glVertex3d(0, 0, 0) 
		glEnd
    )
    
    enableAntialiasing := method(  
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glLineWidth(1)
		//glEnable(GL_LINE_SMOOTH)
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
		glShadeModel(GL_SMOOTH)
		glutMainLoop
    )

    // --- Events ---------------------------------

    setFirstResponder := method(fr, 
		//if (firstResponder, firstResponder releaseFirstResponder)
		firstResponder = fr
    )

    keyboard := method(key, x, y,
		//write("keyboard ", key, "\n")
		firstResponder ?keyboard(key, x, y)
		glutPostRedisplay
    )
    
    special := method(key, x, y, 
		//write("special ", key, "\n")
		firstResponder ?special(key, x, y)
		glutPostRedisplay
    )

    mouse := method(b, s, x, y,
		//write("mouse ", x, ", ", y, "\n")
		y = height - y
		Mouse mouse(b, s, x, y)
		if(firstResponder, firstResponder directMouse)
		if(firstResponder == nil, 
			hv := hitView
			firstResponder = hv
			if(hv, hv mouse)
		)
		display
    )

    motion := method(x, y,
		y = height -y
		Mouse motion(x, y)
		if(firstResponder, 
		//write("motion firstResponder :=  ", firstResponder type, "\n")
		firstResponder motion
		)
    )

    hitView := method(
		subviews reverseForeach(i, view, 
			v := view screenHit
			if(v, return v)
		)
		return nil
    )
)


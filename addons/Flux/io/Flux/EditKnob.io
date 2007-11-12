
EditKnob := View clone do(
	size set(8, 8)
   	halfSize := size / 2
    
    newSlot("editTarget")
    newSlot("syncMethodName")
    newSlot("resizeMethodName")
    
    setEditTarget := method(v,
    	editTarget = v
    	v addListener(self)
    	//v setResizeWidth(101)
    	//v setResizeHeight(101)
    	v superview addSubview(self)
    	setNextResponder(v)
    	setEditPointName("topRight")
    	orderFront
    	self
    )
    
    setEditPointName := method(name, 
     	setResizeMethodName("resizeToSet" .. name asCapitalized)
    	setSyncMethodName(name)
    	syncPosition
    	self   	
    )
    
    stopEditing := method(
    	editTarget removeListener(self)
    	superview removeSubview(self)
    	releaseFirstResponder
    )
    
    draw := method(
		if(isFirstResponder, glColor4d(0, 1, 0, .6), glColor4d(0, 1, 0, .2))
		size drawQuad
		glColor4d(0, 1, 0, .3)
		size drawLineLoopi
    )
    
    leftMouseDown := method(
    	writeln("EditKnob leftMouseDown")
    	setMoveOffset
    	makeFirstResponder
    )
    
    leftMouseMotion := method(
    	moveWithMouse
    	p := superview viewToScreen(position + halfSize)
    	editTarget screenToView(p)
    	editTarget perform(resizeMethodName, p)
    	//editTarget resizeToSetTopRight(p)
    	//editTarget resizeToSetTopLeft(sp)
    	glutPostRedisplay
    )
    
    leftMouseUp := method(
    	releaseFirstResponder
    )
    
    notificationDidChangeSize := method(
    	syncPosition
    )
    
    notificationDidChangePosition := method(
        syncPosition
    )
    
    syncPosition := method(
    	p := vector(0, 0)
    	editTarget viewToScreen(p)
    	superview screenToView(p)
    	setPosition(p += editTarget perform(syncMethodName) -= halfSize)
    )
    
    keyboard := method(key, x, y,
		write(self type, " keyboard\n")
		nextResponder ?keyboard(key, x, y)
    )
    
)

View do(
	topLeft     := method(size clone setX(0))
	topRight    := method(size clone)
	bottomRight := method(size clone setY(0))
	bottomLeft  := method(position clone zero)

	middleRight  := method(s := vector(size x,     size y / 2))
	middleTop    := method(s := vector(size x / 2, size y))
	middleLeft   := method(s := vector(0,          size y / 2))
	middleBottom := method(s := vector(size x / 2, 0))
	
	setPositionInSelfCoords := method(p, setPosition(position + p))

	resizeToSetBottomLeft := method(vp, 
		d := topRight - vp
		setPositionInSelfCoords(vp)
		didChangePosition
		forceResizeTo(d x, d y)
	)

	resizeToSetTopRight := method(vp, 
		forceResizeTo(vp x, vp y)
	)
	
	resizeToSetTopLeft := method(vp,
		sx := (topRight - vp) x
		sy := (vp - bottomRight) y
		setPositionInSelfCoords(vp clone setY(0))
		didChangePosition
		forceResizeTo(sx, sy)
	)
	
	resizeToSetBottomRight := method(vp,
		/*
		sx := (vp - bottomLeft) x
		sy := (topRight - vp) y		
		setPositionInSelfCoords(vp clone setX(0))
		didChangePosition
		forceResizeTo(sx, sy)
		*/
		setPositionInSelfCoords(vp clone setX(0))
		didChangePosition
		s := topLeft - vp
		forceResizeTo(-s x, s y)		
	)
	
	
	resizeToSetMiddleRight := method(vp, 
		sx := (vp - bottomLeft) x
		forceResizeTo(sx, size y)
	)
	
	resizeToSetMiddleLeft := method(vp, 
		sx := (topRight - vp) x
		setPositionInSelfCoords(vp clone setY(0))
		didChangePosition
		forceResizeTo(sx, size y)
	)
	
	resizeToSetMiddleTop := method(vp, 
		forceResizeTo(size x, vp y)
	)
	
	resizeToSetMiddleBottom := method(vp, 
		sy := (topRight - vp) y
		setPositionInSelfCoords(vp clone setX(0))
		didChangePosition
		forceResizeTo(size x, sy)
	)
)

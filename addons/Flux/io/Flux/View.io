
View := Responder clone do(
	appendProto(Notifier)
    superview := nil
    frame := Box clone
    frame size set(100,100)
    position := method(frame origin)
    setPosition := method(p, position copy(p); self)
    setSize := method(s, size copy(s); self)
    size := method(frame size)
    scale := Point clone set(1,1,1)

	topRightPoint := method(
		position + size
	)
    
    fonts := method(Application fonts)
    font ::= fonts system normal
    
    isClipped ::= true
    clippingOn  := method(isClipped = true)
    clippingOff := method(isClipped = false)

    outlineWidth := 1
    debugHit := false

    // Resizing

    height := method(size height)
    width  := method(size width)
    setHeight := method(n, size setHeight(n))
    setWidth  := method(n, size setWidth(n))

    maxSize ::= nil
    minSize ::= vector(0,0)
    resizeWidth  ::= 110
    resizeHeight ::= 110

    backgroundColor ::= Point clone set(1, 1, 1, 1)
    selectedColor   ::= Point clone set(1, 1, 1, 1)
    outlineColor    ::= Point clone set(0, 0, 0, 1)
    subviews  ::= List clone
    superview := nil
    needsRedraw ::= false
    
 	setNeedsRedraw := method(bool,
 		needsRedraw = bool
 		if(needsRedraw, glutPostRedisplay)
 	)
 	
   	isDisplayListCached ::= false
        
    init := method(
		resend
		frame = frame clone
		//scissorIsOn = 0
		subviews = List clone // do this, or end up with a mess
		scale = scale clone
		needsRedraw = true
    )

    // --- Display List -------------------------------------------------

    displayList := method( // don't call this on the root View
		displayList = DisplayList clone
		displayList
    )
    
    doWithinDisplayList := method(
		if(needsRedraw,
			displayList begin
			call evalArgAt(0)
			displayList end
			needsRedraw = nil
		)
		displayList call
    )
    
    // --- Subviews -------------------------------------------------

	Object uniqueName := method(
		self type .. "_" .. self uniqueId
	)
	
    setSuperview := method(view, 
		//writeln(self uniqueName, " setSuperview(", self uniqueName, ")")
		self superview := view; self
	)
	
    topView := method(if(superview, superview topView, self))

    setTopWindow := method(window, 
		//writeln(self uniqueName, " setTopWindow(", window uniqueName, ")")
		topWindow = window
		self
	)
	
    topWindow := method(
		//writeln("topWindow")
		if(superview, 
			//writeln(self uniqueId, " superview topWindow")
			superview topWindow, 
			//writeln(self uniqueName, " no superview")
			nil
		)
	)
    
    addSubview := method(view, 
		subviews append(view)
		view setNextResponder(self)
		view setSuperview(self)
    )
    
    removeSubview := method(view,
		view releaseFirstResponder
		subviews remove(view)
    )
    
    removeFromSuperview := method(
    	superview removeSubview(self)
    	self
    )
    
    removeAllSubviews := method(
    	subviews clone foreach(v, removeSubview(v))
		self
    )

    orderBack := method(
		if(superview, superview subviews remove(self) append(self))
		self
    )

    orderFront := method(
		if(superview, superview subviews remove(self) atInsert(0, self))
		self
    )
    
    /*
    sizeToSubviews := method(
      p := Point clone zero
      subviews foreach(v, p Max(v position + v size))
      size copy(p)
    )
    */

    // --- Display -------------------------------------------------

    display := method(
    	//writeln(self type, "_", self uniqueId, " display")
		if(isClipped, scissorOn)
		glPushMatrix
			position glTranslatei
			scale glScale
			glPushMatrix
			viewDraw
			glPopMatrix
			subviews reverseForeach(i, view, view display)
			if(isClipped, scissorOff)
			drawOutline
			
			if(debugHit and topWindow firstResponder == self, 
                glColor4d(1,0,0,.1)
                size drawQuad
			)
		glPopMatrix
		setNeedsRedraw(false)
    )
    
    viewDraw := method(draw)

    drawUnclipped := method(
		if(GLScissor isOn) then(
			GLScissor off 
			call evalArgAt(0)
			GLScissor on 
		) else(
			call evalArgAt(0)
		)
	)

    draw := nil

    drawBackground := method(
		backgroundColor glColor 
		size drawQuad
    )

	drawLineOutline := method(
		glLineWidth(1)
		outlineColor glColor
		size drawLineLoop
    )
    
    drawOutline := nil

	scissorOn := method(
        GLScissor push
        if(GLScissor isOn, 
            GLScissor unionWithViewRect(frame), 
            GLScissor setViewRect(frame) on)
	)

	scissorOff := method(if(GLScissor isOn, GLScissor pop))

	drawBorderLine := method(x1, y1, x2, y2, r, g, b,
		c := .7
		if(r == nil, r = c)
		if(g == nil, g = c)
		if(b == nil, b = c)
		glPushAttrib(GL_LINE_BIT)
		glDisable(GL_LINE_SMOOTH)
		glLineWidth(outlineWidth)
		glBegin(GL_LINES)
		glColor4d(r, g, b, 1)
		glVertex2i(x1, y1) 
		glVertex2i(x2, y2)
		glEnd
		glPopAttrib
	)

	drawPoppedOutOutline := method(
		w := size width
		h := size height
		
		glLineWidth(outlineWidth)
		glBegin(GL_LINES)
		glColor4d(1, 1, 1, .7)
		glVertex2i(0, 0) 
		glVertex2i(0, h+1)
		
		glVertex2i(0, h)
		glVertex2i(w, h)
		
		glColor4d(0,0,0,.7)
		//glVertex2i(0, h-1, 0)
		//glVertex2i(w, h-1, 0)
		
		glVertex2i(w, h)
		glVertex2i(w, 0)
		
		glVertex2i(w, 0)
		glVertex2i(0, 0)
		glEnd
    )


	drawDepressedOutline := method(
		w := size width
		h := size height
		
		glLineWidth(outlineWidth)
		glBegin(GL_LINES)
		glColor4d(.6, .6, .6, 1)
		glVertex2i(0, 0) 
		glVertex2i(0, h+1)
		
		glColor4d(0, 0, 0, 1)
		glVertex2i(0, h)
		glVertex2i(w, h)
		glColor4d(.8, .8, .8, 1)
		glVertex2i(0, h-1, 0)
		glVertex2i(w, h-1, 0)
		
		glColor4d(.6, .6, .6, 1)
		glVertex2i(w, h)
		glVertex2i(w, 0)
		
		glColor4d(.85, .85, .85, 1)
		glVertex2i(w, 0)
		glVertex2i(0, 0)
		glEnd
    )

    // --- Events -------------------------------------------------

    releaseFirstResponder := method(
        //write(self uniqueId, " releaseFirstResponder")
        //write(self type, " releaseFirstResponder")
        if(topWindow,
            if(topWindow firstResponder == self, 
                topWindow setFirstResponder(nil)
            )
            subviews foreach(releaseFirstResponder)
        )
    )
    
    
    makeFirstResponder := method(
       topWindow setFirstResponder(self)
    )
    
    directHit := method(p,	
    	p = p - position
    	p <= size and p >= zeroPoint
    )
    
    Vector <= := method(other,
        self foreach(i, v, if(other at(i) > v, return false))
        true
    )
    
    Vector >= := method(other,
        self foreach(i, v, if(other at(i) < v, return false))
        true
    )
    
    zeroPoint := vector(0,0,0)
    
    hit := method(p,
    	// p should be in the coords of the superview
		p = p - position
		//write(self type, " hit p = ", p, "\n"); 
		//write("position ", position, "\n"); 
		//write("size ", size, "\n"); 
		//write("hit p - position ", p, "\n"); 
		//write("p >= zeroPoint ", p >= zeroPoint, "\n"); 
		//write("p <= size ", p <= size, "\n"); 
		if(p <= size and p >= zeroPoint,
			subviews foreach(view, 
				v := view hit(p)
				if(v, 
					//write("return subview ", v type, "_", v uniqueId, "\n"); 
					return v
				)
			)
			//write("return ", self type, "_", self uniqueId, "\n\n")
			return self
		) 
		//write("return nil\n\n")
		nil
    )

    screenHit := method(
		p := Mouse tmpPoint copy(Mouse position)
		
		//write(self type, " screenHit mouse ", p x, " ", p y , " "); 
		//write(" pos ", position x, " ", position y, " size ",  size x, " ", size y, "\n"); 
		v := hit(screenToView(p) + position)
		
		//writeln("hit = ", v type)
		//writeln(self type, " screenHit return ", v type, "\n")
		
		return v
    )

    /*
    screenMousePoint := method(
		p := Mouse tmpPoint copy(Mouse position)
		self screenToView(p)
		p
    )
    */

    viewMousePoint := method(
    	//write("Mouse position ", Mouse position, "\n")
		p := Mouse tmpPoint copy(Mouse position)
		self screenToView(p)
		p
    )

    screenToView := method(aPoint,
		if(superview, superview screenToView(aPoint))
		aPoint -= position
		aPoint /= scale
		return aPoint
    )

    viewToScreen := method(aPoint,
		aPoint *= scale
		aPoint += position
		//write("viewToScreen ", aPoint, "\n")
		if(superview, superview viewToScreen(aPoint))
		return aPoint
    )

    keyboard := method(key, x, y,
		//writeln(self type, "_", self uniqueId, " keyboard ", key)
		//writeln("keyboard ", GLUT_KEY_DEL, "-", GLUT_KEY_DELETE)
		if(key == 27, if(self hasSlot("keyboardEscape"), keyboardEscape; return))
		if(key == GLUT_KEY_DEL, if(self hasSlot("keyboardDel"), keyboardDel; return))
		if(key == GLUT_KEY_DELETE, if(self hasSlot("keyboardDelete"), keyboardDelete; return))
		nextResponder ?keyboard(key, x, y)
    )

    special := method(key, x, y,
		//writeln(self type, "_", self uniqueId, " special ", key)
		if(key == GLUT_KEY_LEFT,      if(self hasSlot("keyboardLeftArrow"), keyboardLeftArrow; return))
		if(key == GLUT_KEY_UP,        if(self hasSlot("keyboardUpArrow"), keyboardUpArrow; return))
		if(key == GLUT_KEY_RIGHT,     if(self hasSlot("keyboardRightArrow"), keyboardRightArrow; return))
		if(key == GLUT_KEY_DOWN,      if(self hasSlot("keyboardDownArrow"), keyboardDownArrow; return))
		if(key == GLUT_KEY_PAGE_UP,   if(self hasSlot("keyboardPageUp"), keyboardPageUp; return))
		if(key == GLUT_KEY_PAGE_DOWN, if(self hasSlot("keyboardPageDown"), keyboardPageDown; return))
		if(key == GLUT_KEY_HOME,      if(self hasSlot("keyboardHome"), keyboardHome; return))
		if(key == GLUT_KEY_END,       if(self hasSlot("keyboardEnd"), keyboardEnd; return))
		nextResponder ?special(key, x, y)
    )
    
    mouse := method(
		//d := Mouse doubleClick
		//b := Mouse button
		//s := Mouse state
		sm := Mouse stateMessage
		//writeln(self type, " '", sm name, "' ", self hasSlot(sm name))
		if (self hasSlot(sm name), self doMessage(sm), if(nextResponder, nextResponder mouse))
    )

    motion := method(
		b := Mouse button
		if(b == 0, return ?leftMouseMotion)
		if(b == 1, return ?middleMouseMotion)
		if(b == 2, return ?rightMouseMotion)
    )

    moveWithMouseMotion := method(
		position += Mouse position
		position -= Mouse lastPosition
		position x round
		position y round
		//size x round
		//size y round
		//write("view ", position x, ", ", position y, "\n")
		glutPostRedisplay
    )

    directMouse := method(
        //writeln("directMouse ", Mouse state)
		if(Mouse state == 1, mouse; return)
		v := screenHit
		if(v,
			topWindow setFirstResponder(v)
			v mouse
		,
			topWindow setFirstResponder(nil)
			//if(nextResponder, nextResponder directMouse, topWindow setFirstResponder(nil))
		)
		afterDirectMouse
    )
    
    afterDirectMouse := method(
    	releaseFirstResponder
    )
    
    isFirstResponder := method(topWindow firstResponder == self)

	acceptsFirstResponder ::= false
	
    // --------------------------------------------
	
	resizeWidthWithSuperview  := method(resizeWidth  = 101; self)
	resizeHeightWithSuperview := method(resizeHeight = 101; self)

    resizeWithSuperview := method(
		resizeWidthWithSuperview
		resizeHeightWithSuperview
		self
    )
    
    resizeToFitSuperview := method(
		resizeToSize(superview size)
		self
    )

    doNotResize := method(
		resizeWidth  = 110
		resizeHeight = 110
		self
    )

    resizeToSize := method(size,
		resizeTo(size width, size height)
		self
	)
	 
    resizeTo := method(w, h, 
		resizeBy(w - width, h - height)
    )
    
    resizeWidthTo  := method(w, resizeBy(w - width, 0))
    resizeHeightTo := method(h, resizeBy(0, h - height))

    forceResizeTo := method(w, h, 
		rw := resizeWidth
		rh := resizeHeight
		resizeWidth  = 101
		resizeHeight = 101
		resizeTo(w, h)
		resizeWidth  = rw
		resizeHeight = rh
    )

    resizeXFunc := method(id, dx, x,
		// 1 := fixed, 0 := spring
		if(id == 000, return x + dx/2)
		//if(id == 001, return x - dx)
		if(id == 001, return x + dx/2)
		if(id == 010, return x + dx/2)
		if(id == 011, return x + dx)
		// 110, 111 nop
		x
    )

    resizeWFunc := method(id, dx, w,
		// 1 = fixed, 0 = spring
		if(id == 000, return w + dx/2)
		if(id == 001, return w + dx)
		if(id == 100, return w + dx/2)
		if(id == 101, return w + dx)
		// 110, 111 nop
		w
    )

    resizeBy := method(dx, dy, 
		//writeln(self type, " resizeBy(", dx, ", ", dy, ")")
		x := self resizeXFunc(resizeWidth, dx, position x)
		w := self resizeWFunc(resizeWidth, dx, size width)
	
		y := self resizeXFunc(resizeHeight, dy, position y)
		h := self resizeWFunc(resizeHeight, dy, size height)
	
		if(minSize, w = w max(minSize width); h = h max(minSize height))
		if(maxSize, w = w min(maxSize width); h = h min(maxSize height))
	
		dx := w - size width
		dy := h - size height
		sizeDidChange := dx != 0 or dy != 0
	
		position set(x, y)
		size set(w, h)
	
	    (size += .5) floor
	    (position += .5) floor
	
		if(sizeDidChange, subviews foreach(resizeBy(dx, dy)); didChangeSize)
    )
 
    resizeBy := method(dx, dy, resizeByVector(vector(dx, dy)))
    
    viewName := nil
    
    resizeByVector := method(d,
        outd := frame resizeBy(d clone, resizeWidth, resizeHeight, minSize, maxSize)
		if(outd isZero not, subviews foreach(resizeByVector(outd)); didChangeSize)
    )
        
    didChangeSize := method(
    	//notifiyListeners(notificationDidChangeSize(self))
     	if(?listeners, listeners foreach(?notificationDidChangeSize(self)))
   )
    
    didChangePosition := method(
    	//notifiyListeners(notificationOfReposition(self))
    	if(?listeners, listeners foreach(?notificationDidChangePosition(self)))
    )

    // --------------------------------------------

    alignTopWith := method(otherView, 
		position setY(otherView position y + otherView size y - self size y)
		self
    )
    
    placeAbove := method(otherView, dx,
		dx = if(dx, dx, 10)
		position set(otherView position x, otherView position y + otherView height + dx)
		self
    )
    
    placeBelow := method(otherView, dx,
		dx = if(dx, dx, 10)
		position set(otherView position x, otherView position y - otherView height - dx)
		self
    )
    
    placeRightOf := method(otherView, dx,
		dx = if(dx, dx, 10)
		position set(otherView position x + otherView size x + dx, otherView position y)
		self
    )
    
    placeLeftOf := method(otherView, dx,
		dx = if(dx, dx, 10)
		position set(otherView position x - width - dx, otherView position y)
		self
    )
    
    subviewsBounds := method(
		b := Point clone
		p := Point clone
		subviews foreach(v, 
			p copy(v position) += v size
			//writeln("b size = ", b size)
			//writeln("p size = ", p size)
			p setSize(2)
			b Max(p)
		)
		b
    )    

    sizeToSubviews := method(
		size copy(subviewsBounds)
    )
    
    sizeToSuperview := method(
		position set(0,0)
		if(superview, size copy(superview size))
    )
    
    activeState := 0 // 0 = normal, 1 = active, -1 = disabled
    
    drawBackgroundTextures := method(
		backgroundTextures draw(width, height)
    )
    
	setMoveOffset := method(
		self moveOffset := screenToView(Mouse position clone)    
	)
	
	moveWithMouse := method(
		position copy(Mouse position)
		superview screenToView(position)
		position -= moveOffset
		didChangePosition
	)
    
    editors ::= nil
    
    //toggleEditMode := method(if(editor, endEditMode, beginEditMode))
    
    beginEditMode := method(
    	if(editors == nil, 
    		points := list("bottomLeft", "topRight", "topLeft", "bottomRight", "middleRight", "middleLeft", "middleTop",  "middleBottom")
    		setEditors(points map(name, EditKnob clone setEditTarget(self) setEditPointName(name)))
   		)
    	makeFirstResponder
    )
    
    endEditMode := method(
    	if(editors, editors foreach(stopEditing); editors = nil)
    )
    
	viewChainAttribute := method(name,
		if(self hasSlot(name), return self getSlot(name))
		if(superview, return superview viewChainAttribute(name))
		nil
	)
	
	isTextured := false
	
	quadric := method(self quadric := gluNewQuadric)
	
	roundingSize ::= 10
	boxColor ::= Color clone set(.1, .1, .1, 1)
	backgroundColor ::= Color clone set(0, 0, 0, 1)
	
	drawRoundedBox := method(
        slices := 20
        boxColor glColor
        gluRoundedBox(quadric, size x, size y, roundingSize, slices)
        gluRoundedBoxOutline(quadric, size x, size y, roundingSize, slices)
    )
    
	drawRoundedBoxOutline := method(
        slices := 20
        gluRoundedBoxOutline(quadric, size x, size y, roundingSize, slices)
    )
)


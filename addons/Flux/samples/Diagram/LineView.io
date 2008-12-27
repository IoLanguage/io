LineView := View clone do(
	appendProto(Selectable)

	origin ::= Point clone
	endpoint ::= Point clone

	selectedColor ::= Point clone set(.1, .475, .1, 1)
	lineColor ::= Point clone set(.3, .3, .3, 1)

	isDangling ::= true
	isAttached ::= false

	hitThreshold ::= 6
	
	init := method(
		setOrigin(Point clone)
		setEndpoint(Point clone)
	)
	isLine := true
	//resizeWidthWithSuperview
	//resizeHeightWithSuperview
	
	clippingOff
	
	p1 := method(if(origin ?isBox, origin centerInScreenCoords, origin))
	p2 := method(if(endpoint ?isBox, endpoint centerInScreenCoords, endpoint))
	
	draw := method(
		//writeln("origin ", origin type)
		a := if(origin ?isBox, origin connectPointFor(p2), origin)
		//writeln("endpoint ", endpoint type)
		b := if(endpoint ?isBox, endpoint connectPointFor(p1), endpoint)
		
		if(if(isAttached, isSelected, isDangling not),
			selectedColor glColor
			glLineWidth(2)
		,
			lineColor glColor
			glLineWidth(3)
		)
		glBegin(GL_LINES)
			a glVertex
			b glVertex
		glEnd
		glLineWidth(1)
	)

	delete := method(
		removeFromSuperview
	)
	
	setMoveOffset := nil
	moveOffset := nil
	
	rightMouseMotion := method(
		if(isAttached, return)

		p := superview screenToView(Mouse position)
		hv := superview subviews detect(v, v ?isBox and v directHit(p))
		isDangling = if(hv, false, true)
		if(hv,
			endpoint = hv
		,
			if(endpoint ?isBox,
				endpoint = Point clone
			)
			endpoint copy(screenToView(Mouse position clone))
		)

		setNeedsRedraw(true)
	)
	
	rightMouseUp := method(
		if(isAttached, return)
		p := superview screenToView(Mouse position)
		hv := superview subviews detect(v, v ?isBox and v directHit(p))
		if(hv, setEndpoint(hv), delete)
		isAttached = true
		isDangling = false

		setNeedsRedraw(true)
	)

	shiftLeftMouseDown := method(
		toggleSelection
	)
	
	delete := method(
		removeFromSuperview
	)

	hit := method(p,
		p = p - position
		if((p2 - p1) rot90 dot(p - p1) abs < hitThreshold,
			self
		,
			nil
		)
	)
)

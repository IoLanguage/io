
DBox := View clone do(
	appendProto(Selectable)

	newSlot("label")
	
	init := method(
		resend
		self label := LabelView clone
		label textColor set(1, 1, 1, 1)
		label position set(10, 5)
		label size setWidth(width-10)
		label textColor set(1, 1, 1, .8)
		label setResizeHeight(010)
		label setResizeWidth(101)
		self setMinSize(Point clone set(10, 10))
		addSubview(label)
	)
	
	isBox := true

	size set(140, 30)
	selectedColor ::= Point clone set(.1, .8, .1, 1)
	setOutlineColor(Point clone set(.3, .3, .3, .95))
	setBackgroundColor(Point clone set(.2, .2, .2, 1))


	center := method((size clone *= .5) += position)
	centerInScreenCoords := method(superview viewToScreen(center))
	
	topPoint    := method(center setY(position y + height))
	bottomPoint := method(center setY(position y))
	leftPoint   := method(center setX(position x + width))
	rightPoint  := method(center setX(position x))
		
	connectPointFor := method(c,		
		p := center - c
		p1 := if(p x < 0, leftPoint, rightPoint)
		p2 := if(p y > 0, bottomPoint, topPoint)
		if(p y abs < height, return p1)
		if(p x abs < width + background gridSize, return p2)
		r := if(p1 distanceTo(c) < p2 distanceTo(c), p1, p2)
		superview viewToScreen(r)
	)
	
	drawOutline := method(
		if(isSelected, selectedColor glColor, outlineColor glColor)
		size drawLineLoopi
	)
	
	draw := method(
		backgroundColor glColor 
		size drawQuad
	)
	
	unselect := method(
		resend
		label setIsActive(false)
		updateLineSelection
		endEditMode
	)

	select := method(
		resend
		updateLineSelection
	)

	updateLineSelection := method(
		superview subviews select(?isLine) foreach(line,
			if(line origin isSelected and line endpoint isSelected,
				line select
			,
				line unselect
			)
		)
	)
	
	leftMouseDown := method(
 		//writeln("leftMouseDown")
		if(isSelected not,
			selectOnly
			beginEditMode
		)
		//orderFront
		superview subviews select(?isBox) foreach(setMoveOffset)
		makeFirstResponder
	)
		    
	leftMouseUp := method(alignToGrid)
	
	shiftLeftMouseDown := method(
		toggleSelection
		superview subviews select(?isBox) foreach(setMoveOffset)
	)

	shiftLeftMouseMotion := method(leftMouseMotion)
	
	leftMouseMotion := method(
		superview subviews select(?isBox) select(?isSelected) foreach(moveWithMouse)
		glutPostRedisplay
	)

	doubleLeftMouseDown := method(
		label setIsActive(true)
		label makeFirstResponder
		label leftMouseDown
	)
	
	rightMouseDown := method(
		line := LineView clone setOrigin(self)
		superview addSubview(line)
		line makeFirstResponder
		line rightMouseMotion
		selectOnly
		setNeedsRedraw(true)
	)

	selectOnly := method(
		superview subviews select(?isBox) foreach(unselect)
		select
	)
	
	didChangePosition := method(
		alignToGrid
		resend
	)
	
	resizeTo := method(w, h,
		gridSize := background gridSize
		w = (w / gridSize) roundDown * gridSize
		h = (h / gridSize) roundDown * gridSize
		super(resizeTo(w, h))
	)
    
	keyboard := method(key,
		writeln("DBox keyboard ", key)
		if(key == 24, delete) // control-x
		//8 = delete 1, 127 = delete 2
	)

	deleteConnections := method(
		superview subviews select(v, v ?(isLine) and (v origin == self or v endpoint == self)) foreach(delete)
	)
	
	delete := method(
		endEditMode
		deleteConnections
		removeFromSuperview
	)
    
	background := method(superview viewChainAttribute("background"))

	alignToGrid := method(
		gridSize := background gridSize
		position setX((position x / gridSize) roundDown * gridSize)
		position setY((position y / gridSize) roundDown * gridSize)
	)
	
	chooseRandomPosition := method(
		position setX((superview width - width)* Random value) setY((superview height - height) * Random value)
	)
)


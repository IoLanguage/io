
Background := View clone do(
	gridSize ::= 10
	setBackgroundColor(Point clone set(.1, .1, .1, 1))
	setOutlineColor(Point clone set(1, 1, 1, .03))
	resizeWidthWithSuperview
	resizeHeightWithSuperview
	selectionArea ::= nil

	lastBox ::= nil
	
	doubleLeftMouseDown := method(
		box := DBox clone
		addSubview(box)
		box orderFront
		box setPosition(screenToView(Mouse position) - (box size / 2)) 
		box alignToGrid
		box select
		box beginEditMode
	)
	
	leftMouseDown := method(
		subviews foreach(?unselect)
		makeFirstResponder
		shiftLeftMouseDown
	)

	shiftLeftMouseDown := method(
		if(selectionArea == nil, 
			selectionArea = SelectionArea
			selectionArea position copy(Mouse position)
			selectionArea size set(0, 0)
			addSubview(selectionArea)
			selectionArea makeFirstResponder
			selectionArea orderFront
		)
		glutPostRedisplay
	)
	
	updateSelection := method(
		if(selectionArea, 
			removeSubview(selectionArea)
			subviews foreach(v,
				if(v ?isBox and selectionArea frame containsPoint(v center), v select)
			)
			setSelectionArea(nil)
		)	
	)
	
	leftMouseUp := method(updateSelection)

	shiftLeftMouseUp := method(updateSelection)
	
	rightMouseDown := method(
		writeln("add code to add a line")
	)

	rightMouseUp := method(
	)
	
	keyboard := method(key,
		writeln("Background keyboard")
		if(key == GLUT_KEY_ESC, glutFullScreen) // escape
		if(key == 24, subviews select(?isSelected) foreach(?delete)) // control-x
		//8 = delete 1, 127 = delete 2
	)
	
	acceptsFirstResponder := false
			
	draw := method(
		backgroundColor glColor 
		size drawQuad
		
		outlineColor glColor 
		glLineWidth(1)
		glBegin(GL_LINES)
		for(x, 0, width, gridSize,
			glVertex2d(x, 0)
			glVertex2d(x, height)
		)
		glEnd
		
		glBegin(GL_LINES)
		for(y, 0, height, gridSize,
			glVertex2d(0, y)
			glVertex2d(width, y)
		)
		glEnd
	)
)

Background background := Background



SelectionArea := View clone do(
	setOutlineColor(Point clone set(.1, .8, .1, .9))
	setBackgroundColor(Point clone set(.1, .8, .1, .10))

	drawOutline := method(
		outlineColor glColor
		size drawLineLoopi
	)
		
	draw := method(
		backgroundColor glColor 
		size drawQuad
	)
	
	leftMouseMotion := method(
		p := Mouse position clone
		screenToView(p)
		size copy(p)
		glutPostRedisplay
	)
	
	leftMouseUp := method(superview leftMouseUp)

	shiftLeftMouseUp := method(superview shiftLeftMouseUp)
	
	clippingOff
)

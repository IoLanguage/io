
HKnob := View clone 
HKnob do(
	outlineColor set(.5,.5,.5,1)
	unselectedColor := Point clone set(.2, .2, .2, 1)
	selectedColor   := Point clone set(.5,.5,.5,1)
	backgroundColor = unselectedColor
	size set(30,20)
	resizeWidth  := 010
    resizeHeight := 101
	spacing := 1
	clippingOff
	isSelected ::= false
	
	axis         := "x"
	setAxis      := "setX"
	otherAxis    := "y"
	setOtherAxis := "setY"
	
	makeHorizontal := method(
		axis         = "x"
		setAxis      = "setX"
		otherAxis    = "y"
		setOtherAxis = "setY"
	)
	
	makeVertical   := method(
		axis         = "y"
		setAxis      = "setY"
		otherAxis    = "x"
		setOtherAxis = "setX"
	)
	
	init := method(
		resend
		axis = axis clone
		otherAxis = otherAxis clone
	)
	
	leftMouseDown := method(
        //writeln("HKnob ", self uniqueId, " leftMouseDown")
        backgroundColor = selectedColor
        makeFirstResponder
        setIsSelected(true)
	)
	
	leftMouseUp   := method(
        //writeln("HKnob ", self uniqueId, " leftMouseUp")
        backgroundColor = unselectedColor
        releaseFirstResponder
		superview doAction
        setIsSelected(false)
	)
	
	leftMouseMotion := method(
       //writeln("HKnob ", self uniqueId, " leftMouseMotion")
		vp := superview screenToView(Mouse position clone)
		dp := Mouse position - Mouse lastPosition
		if (vp perform(axis) < 0 and dp perform(axis) > 0, return)
		if (vp perform(axis) > superview size perform(axis) and dp perform(axis) < 0, return)
		position = position + dp
		applyConstraints
		superview updateValue
		superview doAction
		glutPostRedisplay
	)
	
	applyConstraints := method(
		position floor
		position perform(setOtherAxis, 0)
		position perform(setAxis, 
		  position perform(axis) clip(0, superview size perform(axis) - size perform(axis))
	   )
	   //write(self uniqueId, " p: ", position, "\n")
	   //write(self uniqueId, " s: ", size, "\n\n")
	)
	
	maxSpace := 32
	
	textures := TextureGroup clone loadGroupNamed("Scroller/Horizontal/Knob")
	
	draw := method(
        //writeln("HKnob x ", position x, " y ", position y)
        //glColor4d(1,0,0,.5)
        //size drawLineLoopi
        if(isTextured,
            textures draw(width, height)
        ,
            if(isSelected, boxColor copy(selectedColor), boxColor copy(unselectedColor))
            drawRoundedBox
            //glColor4d(.18,.18,.18,1)
            drawRoundedBoxOutline
	    )
	)
		
	drawOutline := nil
)
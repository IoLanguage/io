
HScroller := View clone
    
HScroller do(
	backgroundColor set(1,1,1,.2)
	outlineColor set(.5,.5,.5,1)
	resizeHeight := 010
	resizeWidth := 101
	knob := HKnob clone
	addSubview(knob)
	axis         := "x"
	setAxis      := "setX"
	otherAxis    := "y"
	setOtherAxis := "setY"
	proportion := .1
	value := 0
	clippingOff
	defaultValue := 0
	
	init := method(
		resend
		knob = knob clone
		addSubview(knob)
		//axis = axis clone
		setValue(defaultValue)
	)
	
	knobIsHidden := method(subviews contains(knob) not)
	hideKnob := method(removeSubview(knob))
	
	unhideKnob := method(
		if(knobIsHidden,
            addSubview(knob)
            setValue(defaultValue)
            doAction
    	)
	)
	
	setValue := method(v, value = v; applyConstraints; self)
	
	updateValue := method(
		if(knobIsHidden, value = 1; return)
		d := size perform(axis) - knob size perform(axis)
		if(d == 0,  value = 1; return)
		value = knob position perform(axis) / d
	)
	
	setProportion := method(p,
		proportion = p
		applyConstraints
		//writeln("p = ", p)
		if(p > 1, hideKnob, unhideKnob)
	)
	
	setXProportion := method(p, setProportion(p))
	setYProportion := method(nil)
	
	applyConstraints := method(
		knob position perform(setAxis, value * (size perform(axis) - knob size perform(axis)))
		knob size perform(setAxis, (size perform(axis) * proportion) max(knob maxSpace))
		knob size perform(setOtherAxis, size perform(otherAxis))
		knob applyConstraints
	)
	
	resizeBy := method(dx, dy,
		resend
		applyConstraints
		doAction
		//if(protoName == "VScroller", write(self uniqueId, " ", value, "\n"))
	)
	
	resizeByVector := method(d,
		resend
		applyConstraints
		doAction
		//if(protoName == "VScroller", write(self uniqueId, " ", value, "\n"))
	)
	
	leftMouseDown := method(
		if(knobIsHidden, return)
		p := screenToView(Mouse position clone) perform(axis)
		k := knob size perform(axis)
		max := size perform(axis)
		top := p - k/2
		bottom := max - k
		v := top / bottom max(0.1)
		setValue(v clip(0,1))
		doAction
		knob makeFirstResponder
	)
	
	leftMouseUp := method(doAction)
    	
	leftMouseMotion := nil
	
	textures := TextureGroup clone loadGroupNamed("Scroller/Horizontal")
    
    //boxColor := Color clone set(0,0,0,1)

	draw := method(
        if(isTextured,
            textures draw(width, height)
        ,
            drawRoundedBox
            //drawRoundedBoxOutline
	    )
    )
	
	//size setHeight(leftImage height)
	size setHeight(16)
)





VStackView := View clone do(
	protoName := "VStackView"
	resizeWidth  = 101
	resizeHeight = 110
	
	organizeBottomUp := method(
		last := nil
		
		subviews foreach(v,
			v resizeWidth = 101
			if (last, v placeAbove(last, 0), v position set(0,0))
			last = v
		)
		
		sizeToSubviews
		superview ?subviewDidUpdate
	)
	
	organizeTopDown := method(padding,
		if(padding == nil, padding = 0)
		last := nil
		
		subviews foreach(v,
			v resizeWidth = 101
			if (last, v placeBelow(last, padding), v position set(0, height - v height))
			last = v
		)
		
		superview ?subviewDidUpdate
	)
	
	organize := method(organizeBottomUp)
	
	addSubview    := method(resend; organize)
	removeSubview := method(resend; organize)
    
    draw := method(
        resend
        //glColor4d(0,0,1,.5)
        //size drawLineLoopi
    )
)

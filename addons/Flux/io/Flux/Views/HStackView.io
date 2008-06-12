
HStackView := View clone do(	    
    organize := method(
		last := nil
		
		subviews foreach(v,
			v resizeWidth  = 110
			v resizeHeight = 101
			v resizeHeightTo(height)
			if (last, v placeRightOf(last, 0), v position set(0,0))
			last = v
		)
		
		sizeToSubviews
		setWidth(width + 1)
    )
    
    addSubview    := method(resend; organize)
    removeSubview := method(resend; organize)
)


HTabView := HStackView clone do(
	
	resizeToFitSubviews := method(
		w := 0
		
		subviews foreach(v,
			w = w + v width		
		)
		
		setWidth(w)
		
		setNeedsRedraw(true)
		self
	)
	
	/*
	draw := method(
		Color Red glColor
		glRecti(0, 0, size x, size y) 
	)
	*/
	
	setup := method(
		subviews foreach(v,
			v setActionTarget(self) setAction("selectTab")	
		)
		
	)
	
    addSubview    := method(v, 
		resend
		v setHeight(height)
		resizeToFitSubviews
		self
	)
	
    removeSubview := method(v, 
		resend
		resizeToFitSubviews
		v setActionTarget(nil)
		self
	)

	selectTab := method(tab,
		subviews foreach(setIsActive(false))
		tab setIsActive(true)
		doAction
	)
)

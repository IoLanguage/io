
VTabView := VStackView clone do(
	topAligned := true
	
	organize := method(organizeTopDown(padding))

	resizeToFitSubviews := method(
		newHeight := subviews map(height) sum floor
		//position setY(height-newHeight)
		//setHeight(newHeight)
		setNeedsRedraw(true)
		self
	)
	
	setup := method(
		subviews foreach(setActionTarget(self) setAction("selectTab"))
	)
	
    addSubview := method(v, 
		resend
		v setWidth(width)
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

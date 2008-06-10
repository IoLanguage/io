
VTabView := VStackView clone do(
	allowsMultipleSelection := true
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
		subviews foreach(setActionTarget(self) setAction("didSelectTab"))
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

	didSelectTab := method(tab,
		if(Keyboard shiftKeyIsDown not or allowsMultipleSelection not,
			subviews foreach(setIsActive(false))
		)
		tab setIsActive(true)
		doAction
	)
	
	selectedTabs := method(
		subviews select(isActive)
	)
	
	unselectedTabs := method(
		subviews clone removeSeq(selectedTabs)
	)
	
	tabs := method(subviews)
)


HScrollArea := View clone do(	
	init := method(
		resend
		self scroller := HScroller clone 
		scroller position set(0,0)
		scroller size setWidth(width)
		scroller resizeWidth  := 101
		scroller resizeHeight := 110
		addSubview(scroller)
		scroller setAction("scrollAction")
		scroller setActionTarget(self)
		  
		self clipView := ClipView clone
		clipView position set(0, scroller height)
		clipView size set(width, height - scroller height)
		clipView resizeWidth  := 101
		clipView resizeHeight := 101
		addSubview(clipView)
		
		clipView setScrollerDelegate(scroller) 
	)
	
	scrollAction := method(scroller,
		clipView setXOffset(scroller value)
	)
)

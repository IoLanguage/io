
VScrollArea := View clone do(
    protoName := "VScrollArea"
    resizeWidth  := 101
    resizeHeight  := 101
    
    init := method(
		resend
		self scroller := VScroller clone 
		scroller position set(width - scroller width,0)
		scroller size setHeight(height)
		scroller resizeWidth  := 011
		scroller resizeHeight := 101
		addSubview(scroller)
		scroller setAction("scrollAction")
		scroller setActionTarget(self)
		
		self clipView := ClipView clone
		clipView position set(0, 0)
		clipView size set(width - scroller width, height)
		clipView resizeWidth  := 101
		clipView resizeHeight := 101
		addSubview(clipView)
		
		clipView setScrollerDelegate(scroller) 
    )

    scrollAction := method(scroller,
		clipView setYOffset(scroller value)
    )
)
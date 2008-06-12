
VScrollArea := View clone do(
    resizeWidth  := 101
    resizeHeight  := 101
    contentView ::= nil

    init := method(
		resend
		setHeight(100)
		setWidth(100)
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
		clipView resizeWithSuperview
		addSubview(clipView)
		clipView setScrollerDelegate(scroller) 
    )

    scrollAction := method(
		clipView setYOffset(scroller value)
    )
)
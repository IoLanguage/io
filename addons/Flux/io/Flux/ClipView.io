
ClipView := View clone do(
    offset := Point clone 
    
    scrollerDelegate := nil
    setScrollerDelegate := method(d, 
		scrollerDelegate = d; 
		didScroll
	)
    isClipped := true
    invertY := true
    outlineColor = Color clone set(0,0,1,1)
    
    init := method(
		resend
		offset = offset clone
		setDocumentView(testDocumentView)
		backgroundColor set(1,0,1,.5)
    )

    documentView := method(subviews at(0))

    setDocumentView := method(v, 
		removeAllSubviews 
		addSubview(v)
    )

    setXOffset := method(x, 
		offset setX(x clip(0,1))
		didScroll
    )
    
    setYOffset := method(y, 
		offset setY(y clip(0,1))
		didScroll
    )

    xRatio := method(size x / documentView size x)
    yRatio := method(size y / documentView size y)
    
	update := method(didScroll)
    
	positionOffset := method(
		if(xRatio > 1, offset setX(0))
		if(yRatio > 1, offset setY(0))
		d := size - documentView size 
		d *= offset		
		if (invertY and documentView size y < size y, d setY(d y + height - documentView size y))
		d
	)

	didScroll := method(
		documentView ?didScroll
		documentView position copy(positionOffset) floor
		//writeln("didScroll")
		documentView setNeedsRedraw(true)
    )

    testDocumentView := View clone do(
		position set(0,0)
		size set(800,1000)
		//textures := TextureGroup clone loadGroupNamed("TestPattern")
		draw := method(
			//textures draw(width, height)
			glColor4d(1,0,0,1)
			glBegin(GL_LINES)
			glVertex2i(0,0)
			glVertex2i(width, height)
			glVertex2i(width, 0)
			glVertex2i(0, height)
			glEnd
			glColor4d(0,0,0,1)
		)
    )

	didChangeSize := method(
		resend
		//writeln("clipView didChangeSize")
		documentView didChangeSize
		if (scrollerDelegate, 
			//write("setXProportion(", xRatio, ")\n")
			//write("setYProportion(", yRatio, ")\n")
			scrollerDelegate ?setXProportion(xRatio)
			scrollerDelegate ?setYProportion(yRatio)
		)
	)
    
    subviewDiddidScroll := method(
		didScroll
    )
)

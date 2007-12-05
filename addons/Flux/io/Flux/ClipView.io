
ClipView := View clone do(
    protoName := "ClipView"
    offset := Point clone 
    
    scrollerDelegate := nil
    setScrollerDelegate := method(d, scrollerDelegate = d; update)
    isClipped := 1
    invertY := 1
    outlineColor = Color clone set(0,0,0,1)
    
    init := method(
		resend
		offset = offset clone
		setDocumentView(testDocumentView)
		//backgroundColor set(1,0,1,.5)
    )

    documentView := method(subviews at(0))

    setDocumentView := method(v, 
		removeAllSubviews 
		addSubview(v)
    )

    setXOffset := method(x, 
		offset setX(x clip(0,1))
		update
    )
    
    yoffset := 0
    setYOffset := method(y, 
		//write("setYOffset (", y , ")\n")
		yoffset = y
		offset setY(y clip(0,1))
		update
    )

    xRatio := method(size x / documentView size x)
    yRatio := method(size y / documentView size y)
    
    update := method(
		if (xRatio > 1, offset setX(0))
		if (yRatio > 1, offset setY(0))
		d := size - documentView size 
		d *= offset 
		documentView position copy(d)
		
		if (invertY == 1 and scrollerDelegate proto == VScroller,
		    // nil, 
			documentView position setY((height - documentView height) * yoffset)
			//write("invertY: offset y = ", offset y, " ", yoffset, "\n")
			//write("documentView position y = ", documentView position y, "\n\n")
			//documentView position setY(yRatio)
			//documentView position setY(documentView height - documentView position y)
			//write("documentView position y = ", documentView position y, "\n")
			//write("documentView size x = ", documentView size x, "\n\n")
		)
		
		/*
		if (documentView ?isTrackMeter, write("isTrackMeter:\n"))
		write("size x              = ",  size x, "\n")
		write("documentView size x = ", documentView size x, "\n\n")
		*/
		if (scrollerDelegate, 
			//write("setProportion(", xRatio, ")\n")
			scrollerDelegate ?setXProportion(xRatio)
			scrollerDelegate ?setYProportion(yRatio)
		)
		//write("documentView size = ", documentView size, "\n")
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
    
    resizeBy := method(dx, dy, resend; update)
    
    subviewDidUpdate := method(
		//write("subviewDidUpdate\n")
		update
    )
    
    //drawOutline := method(drawLineOutline)
)

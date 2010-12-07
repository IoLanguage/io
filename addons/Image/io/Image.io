Image do(
	appendProto(Cairo)

	newSlot("path")

	_open := getSlot("open")
	open := method(p,
		if(p, setPath(p))
		_open(path)
	)

/*
	glFormat := method(if(componentCount == 4, GL_RGBA, GL_RGB))
	dataType := method(GL_UNSIGNED_BYTE)
*/

	//doc Image resizedTo(newWidth, newHeight) Scales the image up to newWidth x newHeight.  Returns the newly scaled image.
	resizedTo := method(w, h,
		writeln("resizing ", path, " from ", width, "x", height, " to ", w, "x", h)
		addAlpha
		inputSurface := ImageSurface createForData(data, FORMAT_ARGB32, width, height, width * componentCount)
		outputSurface := ImageSurface create(FORMAT_ARGB32, w, h)
		Context create(outputSurface) scale(w / width, h / height) setSourceSurface(inputSurface, 0, 0) paint

		imgOut := Image clone
		imgOut setDataWidthHeightComponentCount(outputSurface getData, outputSurface getWidth, outputSurface getHeight, self componentCount)
		imgOut
	)
	
	grabScreen := method(
		data := Sequence clone 
		width  := glutGet(GLUT_WINDOW_WIDTH)
		height := glutGet(GLUT_WINDOW_HEIGHT)
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data)	
		self setDataWidthHeightComponentCount(data, width, height, 4)
		self flipY
	)
	
	sizeInBytes := method(
		width * height * componentCount
	)
	
	
	with := method(path,
		self clone setPath(path)
	)

	isWhiteAt := method(x, y,
		i := (x + (width * y)) * componentCount
		r := data at(i + 0)
		g := data at(i + 1)
		b := data at(i + 2)
		(r > 200) and(g > 200) and(b > 200) 
	)

	hasWhiteBackground := method(
		//writeln("isWhiteAt(width-1,   height/4) = ", isWhiteAt(width-1, height/4))
		//writeln("isWhiteAt(width/4, 0)        = ", isWhiteAt(width/4, 0))
		(isWhiteAt(width/4, 0) and isWhiteAt(width*3/4, 0)) or(
			isWhiteAt(width-1, height/4) and isWhiteAt(width-1, height*3/4))
	)
)


Image do(
	newSlot("path")

	_open := getSlot("open")
	open := method(p,
		if(p, setPath(p))
		_open(path)
	)
	
	glFormat := method(if(componentCount == 4, GL_RGBA, GL_RGB))
	dataType := method(GL_UNSIGNED_BYTE)
	
	resizedTo := method(w, h,
		dataOut := Sequence clone 
		gluScaleImage(glFormat, width, height, dataType, data, w, h, dataType, dataOut)
		imgOut := Image clone 
		imgOut setDataWidthHeightComponentCount(dataOut, w, h, self componentCount)		
		imgOut
	)
)

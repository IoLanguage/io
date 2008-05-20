
Image do(
	appendProto(OpenGL)

	IsPowerOf2 := method(x,
		x != 0 and (x & (x - 1)) == 0
	)

	changed := true

	_setDataWidthHeightComponentCount := getSlot("setDataWidthHeightComponentCount")
	setDataWidthHeightComponentCount = method(data, w, h, componentCount,
		_setDataWidthHeightComponentCount(data, w, h, componentCount)
		changed = true
	)

	glFormats := list(nil, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA)

	glFormat := method(
		glFormats at(componentCount)
	)

	sizeIsPowerOf2 := method(IsPowerOf2(width) and IsPowerOf2(height))

	draw := method(
		# Draw the image as a pixmap.
		glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, width)

		glPushMatrix
		// flip the y axis since y=0 starts at first byte of image data
		glRasterPos2d(0, height)
		glPixelZoom(1, -1)
		glDrawPixels(width, height, glFormat, GL_UNSIGNED_BYTE, data)
		glPixelZoom(1, 1)
		glPopMatrix

		glPopClientAttrib

		self
	)

	_texture := lazySlot(Texture clone)

	updateTexture := method(
		_texture uploadImage(self)
		changed = false
		self
	)

	texture := method(
		if (changed, updateTexture)
		_texture
	)

	textureWidth := method(texture width)
	textureHeight := method(texture width)

	drawTexture := method(w, h,
		texture draw(w, h)
		self
	)

	drawScaledTexture := method(w, h,
		texture drawScaled(w, h)
		self
	)
	
	scaledTo := method(widthOut, heightOut,
		dataSeqOut := Sequence clone setSize(widthOut * heightOut * componentCount)
		OpenGL gluScaleImage(self glFormat, width, height, self glFormat, data, widthOut, heightOut, self glFormat, dataSeqOut)
		out := Image clone 
		out setDataWidthHeightComponentCount(dataSeqOut, widthOut, heightOut, self componentCount)
		//writeln("out = ", out type)
		out
	)
)

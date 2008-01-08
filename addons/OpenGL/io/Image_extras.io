isPowerOf2 := method(x,
	x != 0 and (x & (x - 1)) == 0
)

Image do(
	appendProto(OpenGL)

	changed := true

	_setDataWidthHeightComponentCount := getSlot("setDataWidthHeightComponentCount")
	setDataWidthHeightComponentCount = method(data, w, h, componentCount,
		_setDataWidthHeightComponentCount(data, w, h, componentCount)
		changed = true
	)

	glFormat := method(
		if (componentCount == 1, return(GL_LUMINANCE))
		if (componentCount == 2, return(GL_LUMINANCE_ALPHA))
		if (componentCount == 3, return(GL_RGB))
		if (componentCount == 4, return(GL_RGBA))
		nil
	)

	sizeIsPowerOf2 := method(isPowerOf2(width) and isPowerOf2(height))

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
)

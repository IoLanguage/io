
ImageView := View clone do(
	position setX(0) setY(0)
	size setWidth(100) setHeight(24)
	newSlot("delegate")
	newSlot("image")


	init := method(
		resend
		image = Image clone
	)

	sizeToImage := method(
		if(image, size setWidth(image width) setHeight(image height))
		self
	)
	
	draw := method(
		if(image, drawProportional)
	)
	
	drawColor := method(
		glColor4d(1,1,1,1)
	)
	
	drawProportional := method(
		glPushMatrix
		
		wr := width / image originalWidth
		hr := height / image originalHeight
		
		
		if (wr < hr) then(
			glTranslated(0, (height - (image originalHeight * wr))/2, 0)
			glScaled(wr, wr, 1)
		) else(
			glTranslated((width - (image originalWidth * hr)) / 2, 0, 0)
			glScaled(hr, hr, 1)
		)
		
		drawColor
		image drawTexture
		
		//if(isSelected and superview isFirstResponder, drawBorder)

		glPopMatrix

		self
	)

	newSlot("borderColor", Color clone set(1,1,1,.5))
	newSlot("borderSize", 5)
		
	drawBorder := method(
		b := borderSize
		w := image originalWidth
		h := image originalHeight - b
		
		borderColor glColor
		glRecti(0, 0, w, b) // bottom
		glRecti(0, h-b, w, h) // top
		glRecti(0, b, b, h-b) // left
		glRecti(w-b, b, w, h-b) // right

	)
	
	newSlot("isSelected", false)
	select := method(setIsSelected(true))
	unselect := method(setIsSelected(false))
	acceptsFirstResponder := false
	//setBackgroundColor(Color clone set(1, 1, 1, 1))
	
	//drawBackground := nil
)

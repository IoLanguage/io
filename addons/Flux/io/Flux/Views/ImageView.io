
ImageView := View clone do(
	position setX(0) setY(0)
	size setWidth(100) setHeight(24)
	delegate ::= nil
	image ::= nil

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
	
	translateToPlacement := method(
        wr := width / image width
		hr := height / image height
		if (wr < hr) then(
		    self tx := 0
		    self ty := (height - image height * wr)/2
		    self tr := wr
		) else(
            self tx := (width  - image width * hr) / 2
		    self ty := 0
		    self tr := hr
		)
        glTranslated(tx, ty, 0)
        glScaled(tr, tr, 1)
	)
	
	drawProportional := method(
		glPushMatrix
		translateToPlacement
		drawColor
		glColor4d(1, 1, 1, 1)
		writeln("image drawTexture")
		image drawTexture
		if(isSelected and superview isFirstResponder, drawBorder)
		glPopMatrix
		self
	)

	borderColor := Color clone set(1,1,1,1)
	borderSize ::= 1

    setIsClipped(false)
   
	drawBorder := method(
		glPushMatrix
		b := borderSize
		w := image width
		h := image height - b
		
		borderColor glColor
		glBegin(GL_LINE_LOOP)
		glVertex2d(0,0)
		glVertex2d(0,h)
		glVertex2d(w,h)
		glVertex2d(w,0)
        glEnd		
		/*
		glRecti(0, 0, w, b) // bottom
		glRecti(0, h-b, w, h) // top
		glRecti(0, b, b, h-b) // left
		glRecti(w-b, b, w, h-b) // right
		*/
		glPopMatrix
	)
	
	isSelected ::= false
	select := method(setIsSelected(true))
	unselect := method(setIsSelected(false))
	acceptsFirstResponder := false
	//setBackgroundColor(Color clone set(1, 1, 1, 1))
	
	//drawBackground := nil
)

//Collector setDebug(true)

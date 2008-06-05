
MovieView := View clone do(
	position setX(0) setY(0)
	isSelected := 0
	size setWidth(100) setHeight(100)
	motionAction := nil
	movie ::= nil
	
	sizeToMovie := method(
		if(movie, self size setWidth(movie width) setHeight(movie height))
		//movie setOutputWidth(movie width) setOutputHeight(movie height)
		self
	)
	
	draw := method(
		movie draw
		if(isSelected == 1,
			glColor4d(0, 0, 0, .1)
			size drawQuad
		)
	)
	
	timer := method(
		if(movie, 
			movie nextFrame
			if (movie isAtEnd, movie rewind)
			if (movie error, write(movie error, "\n"); exit)
		)
	)
	
	drawBackground := method(self)
	motion := method(self)
)

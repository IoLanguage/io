
Label := View clone do(
	textColor := Point clone set(1, 1, 1, 1)
	title ::= "Label"
	clippingOff
	
	draw := method(
		glPushMatrix
		textColor glColor
		font drawString(title)
		glPopMatrix
	)
	
	setWidthToTitle := method(setWidth(font widthOfString(title)))
	
	drawBackground := nil
	mouse  := nil
	motion := nil
)

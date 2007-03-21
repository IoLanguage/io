
Label := View clone do(
	protoName := "Label"
	textColor := Point clone set(1, 1, 1, 1)
	title := "Label"
	setTitle := method(t, title = t)
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

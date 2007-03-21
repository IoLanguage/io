
TextCell := Button clone do(
	arrow := ImageManager item("Arrows/Right")
	setHeight(20)
	resizeWidth = 101
	selectedColor := Color clone set(1,1,1,.3)
	
	draw := method(
		if(isSelected == 1, selectedColor glColor; size drawQuad)
		glPushMatrix
		textColor glColor
		glTranslated(3, 4, 0)
		font drawString(title)
		glPopMatrix
		if(object proto hasSlot("hasSubitems") == nil, write("object = ", object, "\n") )
		if(object hasSubitems,
            glPushMatrix
            glTranslated(width - 16, 2, 0)
            arrow drawTexture
            glPopMatrix
		)
	)
	
	object := nil
	
	setObject := method(obj,
		object = obj
		setTitle(object title)
		self
	)
	
	leftMouseDown := method(doAction)
	leftMouseUp := nil
	
	select := method(isSelected = 1)
	unselect := method(isSelected = nil)
	
	leftMouseDoubleClickDown := method(
		write("leftMouseDoubleClick\n")
		actionTarget ?itemDoubleClick(self)
	)
)
	
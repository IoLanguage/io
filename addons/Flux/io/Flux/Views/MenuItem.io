
MenuItem := View clone do(	
	title ::= "MenuItem"
	setActionTarget(nil)
	setAction(nil)
	tag := nil
	isSelected := 0
	isTmpSelected := 0
	
	select      := method(isSelected = 1)
	unselect    := method(isSelected = 0)
	tmpSelect   := method(isTmpSelected = 1)
	tmpUnselect := method(isTmpSelected = 0)
	
	checkImage      := ImageManager item("Check")
	checkWhiteImage := ImageManager item("CheckWhite")
	
	draw := method(
		//write("MenuItem draw ", position x, " ", position y, "\n")
		if (isTmpSelected == 1,
			glColor4d(0,0,0,.2)
			size drawQuad
		)
		
		if (isSelected == 1, 
			glPushMatrix
			glTranslated(5, 5, 0)
			if(isTmpSelected == 1, checkWhiteImage draw, checkImage draw)
			glPopMatrix
		)
		
		glPushMatrix
		if (isTmpSelected == 1, glColor4d(1,1,1,1), glColor4d(0,0,0,1))
		glTranslated(20, 5, 0)
		font drawString(title)
		glPopMatrix
	)
	
	mouse := method(
		self isSelected := if(Mouse state == 1, 0, 1)
		glutPostRedisplay
	)
	
	motion := method(Nop)
)

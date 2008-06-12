
PopUpButton := View clone do(	
	position setX(100) setY(100)
	isSelected := 0
	leftEnd    := ImageManager item("PopUpRightEnd")
	rightEnd   := ImageManager item("PopUpRightEnd1")
	fill       := ImageManager item("PopUpFill")
	size setWidth(100) setHeight(25)
	setActionTarget(nil)
	action := nil
	isOpen := 0
	menu := Menu clone
	
	addItemNamed    := method(name, menu addItemNamed(name))
	selectItemNamed := method(name, menu selectItemNamed(name))
	
	init := method(
		resend
		menu = Menu clone
		isClipped = 0
	)
	
	draw := method(
		w := size width
		h := size height
		menu itemSize setWidth(size width)
		
		// draw left PopUpButton cap
		glPushMatrix
		glTranslated(0, 0, 0)
		leftEnd draw
		glPopMatrix
		
		c  := .85
		sh := h/2
		ww := 10
		
		glColor4d(1, 1, 1, 1)
		glRectd(10,3, w-10, h-3)
		
		// draw bottom middle shading
		glBegin(GL_QUADS)
		glColor4d(c, c, c, 1)
		glVertex3d(ww, 3, 0) 
		glColor4d(1, 1, 1, 1)
		glVertex3d(ww, sh, 0)
		glVertex3d(w-ww, sh, 0)
		glColor4d(c, c, c, 1)
		glVertex3d(w-ww, 3, 0)
		glEnd
		/*
		// draw right PopUpButton cap
		glPushMatrix
		glTranslated(10, -1, 0)
		glEnable(GL_TEXTURE_2D)
		glColor4d(1,1,1,1)
		fill drawTexture(w-20, h)
		glDisable(GL_TEXTURE_2D)
		glPopMatrix
		*/
		
		// draw right PopUpButton cap
		glPushMatrix
		glTranslated(w - rightEnd width, 1, 0)
		rightEnd draw
		glPopMatrix
		
		// bottom lines
		self drawBorderLine(8, 3, w - 15, 3, .7, .7, .7)
		self drawBorderLine(8, 2, w - 15, 2, .8, .8, .8)
		self drawBorderLine(8, 1, w - 15, 1, .9, .9, .9)
		
		// top lines
		self drawBorderLine(8, h-3, w - 15, h-3, .8, .8, .8)
		
		// draw label
		if (menu selectedItem,
			glPushMatrix
			glColor4d(0,0,0,1)
			glTranslated(13, 8, 0)
			//glColor4d(0,0,0,.3)
			//jitterDraw(font drawString(menu selectedItem title))
			font drawString(menu selectedItem title)
			glPopMatrix
		)
	)
	
	
	drawBackground := method(self)
	
	open := method(
		if (topView subviews contains(menu) not, 
			menu position copy(viewToScreen(Point clone))
			menu alignWithSelection
			topView addSubview(menu)
		)
	)
	
	close := method(
		if (topView subviews contains(menu), topView removeSubview(menu))
	)
	
	mouse := method(
		menu mouse
		if(Mouse state == 1) then(
			close
		) else (
			open
			menu motion
		)
		glutPostRedisplay
	)
	
	motion := method(
		menu motion
	)

)

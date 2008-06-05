
ScrollBar := View clone do(
	scrollVertCaps   := ImageManager item("ScrollVertCaps")
	scrollVertTop    := ImageManager item("ScrollVertTop")
	scrollVertBottom := ImageManager item("ScrollVertBottom")
	
	size setHeight(300) setWidth(scrollVertBottom width)
	value := 0
	minValue := 0
	maxValue := 1
	ratio := 0
	isSelected := 0
	
	draw := method(
		w := size width
		h := size height
		
		drawBackground
		
		drawBorderLine(w, 0, w, h)
		
		scrollVertBottom draw
		
		glPushMatrix
		glTranslated(0, h - scrollVertTop height, 0)
		scrollVertTop draw
		glPopMatrix
		
		glPushMatrix
		glTranslated(0, barY, 0)
		scrollVertCaps draw
		glPopMatrix
	)
	
	drawBackground := method(
		w := size width
		h := size height 
		
		glBegin(GL_QUADS)
		c := .85
		glColor4d(c, c, c, 1)
		glVertex3d(0, 0, 0) 
		glVertex3d(0, h, 0)
		glColor4d(1, 1, 1, 1)
		glVertex3d(w, h, 0)
		glVertex3d(w, 0, 0)
		glEnd
	)
	
	buttonClick := method(
		"buttonClick\n" print
		glutPostRedisplay
	)
	
	barY := method(
		(size height - scrollVertCaps height) * value
	)
	
	mouse := method(
		vy := screenToView(Mouse position) y
		write("vy := ", vy, "\n")
		write("barY := ", barY, "\n")
		self downY := vy  - barY
		write("downY := ", downY, "\n")
	)
	
	motion := method(
		write("motion\n")
		isSelected = if(Mouse state == 1, 0, 1)
		
		//write("downY := ", downY, "\n")
		value = viewMousePoint y min(size height - 4) max(33) / size height
		write("value = ", value, "\n")
		glutPostRedisplay
	)
)

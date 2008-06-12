
ImageWell := View clone do(	
	position setX(0) setY(0)
	isSelected := 0
	cornersImage := ImageManager item("ImageWellCorners")
	leftRightImage := ImageManager item("ImageWellLeftRight")
	topBottomImage := ImageManager item("ImageWellTopBottom")
	corners := ImageManager item("ImageWellCorners")
	size setWidth(150) setHeight(150)
	motionAction := nil
	image ::= nil
		
	draw := method(
		w := size width
		h := size height
		
		// bottom left cap
		glPushMatrix
		glTranslated(0, 0, 0)
		cornersImage draw
		glPopMatrix
		
		// bottom right cap
		glPushMatrix
		glTranslated(w - cornersImage width-1, 0, 0)
		cornersImage draw
		glPopMatrix
		
		// top left cap
		glPushMatrix
		glTranslated(0, h - cornersImage height, 0)
		cornersImage draw
		glPopMatrix
		
		// bottom right cap
		glPushMatrix
		glTranslated(w - cornersImage width-1, h - cornersImage height, 0)
		cornersImage draw
		glPopMatrix
		
		ww := 9
		hh := 9
		
		// left-right white
		glColor4d(1, 1, 1, 1)
		glRectd(0, hh, w, h-hh)
		
		// top-bottom white
		glColor4d(1,1,1, 1)
		glRectd(ww, 0, w-ww, h)
		
		// left-right gray
		glColor4d(.95, .95, .95 ,1)
		glRectd(ww, 3, w-ww, h-3)
		
		// left-right gray
		glColor4d(.95, .95, .95 ,1)
		glRectd(3, hh, w-3, h-hh)
		
		// top mid gray
		glColor4d(.9, .9, .9 ,1)
		glRectd(ww, h-3, w-ww, h-9)
		
		// draw shading
		glBegin(GL_QUADS)
		glColor4d(.95, .95, .95, 1)
		glVertex3d(ww, h-7, 0) 
		glColor4d(.5, .5, .5, 1)
		glVertex3d(ww, h-3, 0)
		glVertex3d(w-ww, h-3, 0)
		glColor4d(.95, .95, .95, 1)
		glVertex3d(w-ww, h-7, 0) 
		glEnd
		/*
		// draw shading
		glBegin(GL_QUADS)
		glColor4d(.95, .95, .95, 1)
		glVertex3d(3, hh, 0) 
		glColor4d(.9,.9,.9, 1)
		glVertex3d(3, h-9, 0)
		glVertex3d(w-3, h-9, 0)
		glColor4d(.95, .95, .95, 1)
		glVertex3d(w-3, hh, 0) 
		glEnd
		*/
		
		if (image, 
			glPushMatrix
			glTranslated((width - image width)/2, (height - image height)/2, 0)
			image draw
			glPopMatrix
		)
		
		// draw bottom lines
		self drawBorderLine(ww-4, 0, w - ww+4, 0, .9, .9, .9)
		self drawBorderLine(ww-4, 3, w - ww+4, 3, .7, .7, .7)
		
		// draw top lines
		self drawBorderLine(ww-3, h-1, w - ww+3, h-1, .9, .9, .9)
		self drawBorderLine(ww, h-4, w - ww, h-4, .5, .5, .5)
		
		// draw left lines
		self drawBorderLine(1, hh-4, 1, h-hh+6, .9, .9, .9)
		self drawBorderLine(4, hh, 4, h-hh+2, .7, .7, .7)
		self drawBorderLine(5, hh, 5, h-hh+2, .8, .8, .8)
		
		// draw right lines
		self drawBorderLine(w, hh-4, w, h-hh+6, .9, .9, .9)
		self drawBorderLine(w-3, hh, w-3, h-hh+3, .7, .7, .7)
		self drawBorderLine(w-4, hh, w-4, h-hh+4, .8, .8, .8)  
	)
	
	drawBackground := method(nil)
	mouse := method(nil)
	motion := method(nil)
)

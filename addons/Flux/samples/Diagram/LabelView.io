LabelView := TextField clone do(
	textXOffset := 0
	font := fonts system bold
	//textColor := textColor clone set(.6,.6,.6,1)
	draw := method(
		glTranslated(textXOffset, 0, 0)
		h := height - 4
		if(isFirstResponder,
			x1 := cursorX
			glPushMatrix
			if(endCursorPosition) then(
				x2 := cursorX2
				selectionColor glColor
				glBegin(GL_QUADS)
				glVertex2i(x1, 1)
				glVertex2i(x1, h)
				glVertex2i(x2, h)
				glVertex2i(x2, 1)
				glEnd
			) else(
				textColor glColor
				glBegin(GL_LINES)
				glVertex2i(x1, 1)
				glVertex2i(x1, h)
				glEnd
			)
			glPopMatrix
		)

		glTranslated(0,5, 0)
		textColor glColor
		font drawString(renderString)
	)

	isActive ::= false

	leftMouseDown := method(
		if(isActive, resend, superview ?leftMouseDown)
	)

	releaseFirstResponder := method(
		resend
		setIsActive(false)
	)
)


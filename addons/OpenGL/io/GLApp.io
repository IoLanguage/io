OpenGL appendProto(GLU) appendProto(GLUT)

Font appendProto(OpenGL)

Font drawCenteredString := method(text, width,
	glTranslatei(((width - self widthOfString(text)) / 2), 0, 0)
	drawString(text)
)

Font drawOffsetString := method(text, offset,
	glTranslatei(offset, 0, 0)
	drawString(text)
)

GLApp := Object clone do(
	//metadoc GLApp category Graphics
	appendProto(OpenGL)
	newSlot("width", 1024)
	newSlot("height", 768)

	didReshape := nil

	reshape := method(w, h,
		height = h
		width = w
		glViewport(0, 0, w, h)
		glLoadIdentity
		gluOrtho2D(0, w, 0, h)
		didReshape
		glutPostRedisplay
	)

	draw := nil

	backgroundColor := vector(0,0,0,1)

	appDidInit := nil
	didInit := false

	display := method(
		if(didInit not, didInit = true; appDidInit)
		backgroundColor glClearColor
		glClear(GL_COLOR_BUFFER_BIT)
		glPushMatrix
		draw
		glPopMatrix
		glFlush
		glutSwapBuffers
	)

	keyboard := method(k, x, y,
		GLUT_KEY_ESC := 27
		if (k == GLUT_KEY_ESC,    self ?escapeKey)
		if (k == GLUT_KEY_SPACE,  self ?spaceKey)
		if (k == GLUT_KEY_RETURN, self ?returnKey)
		display
	)

	special := method(c, x, y,
		if (c == GLUT_KEY_UP,   self ?specialUpKey)
		if (c == GLUT_KEY_DOWN, self ?specialDownKey)
		display
	)

	escapeKey := method(toggleFullScreen)

	mouse := method(nil)

	timer := method(nil)

	newSlot("appName", "io")

	didToggleFullScreen := nil

	toggleFullScreen := method(

		if(isFullScreen) then(
			glutReshapeWindow(oldWidth, oldHeight)
			glutPositionWindow(oldX, oldY)
		) else(
			self oldX := glutGet(GLUT_WINDOW_X)
			self oldY := glutGet(GLUT_WINDOW_Y)
			self oldWidth := width
			self oldHeight := height
			glutFullScreen
		)

		didToggleFullScreen
		isFullScreen = isFullScreen not
	)

	runFullScreen := method(
		self isFullScreen := true
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
		glutInit
		glutGameModeString("1600x1200:32@60")
		glutEnterGameMode
		initEvents
		initBlending
		glutMainLoop
	)

	run := method(
		self isFullScreen := false
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
		glutInitWindowSize(width, height)
		glutInit
		self glutWindowId := glutCreateWindow(appName)
		glutReshapeWindow(width, height)
		initEvents
		initBlending
		glutMainLoop
	)

	initBlending := method(
		glColor4d(0, 0, 0, 1)
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
	)

	initEvents := method(
		glutEventTarget(self)
		glutDisplayFunc
		glutReshapeFunc
		glutKeyboardFunc
		glutSpecialFunc
		glutMouseFunc
		glutTimerFunc(0, 0)
	)
	
	takeScreenShot := method(path,
		if(path == nil, path = "screenshot.jpg")
		Image clone grabScreen save(path)
	)
)


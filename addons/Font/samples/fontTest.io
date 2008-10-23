#!/usr/bin/env io

// fontTest.io
// Written by Jon Kleiser, 12-Dec-2006
// Based on IoDesktop/FreeType/_tests/test.io

// For the script to find the fonts, first cd to the io distribution
// directory containing addons.

Screen := Object clone do(
	appendProto(OpenGL)
	winHeight := 0
	infoFont := nil
	info := "Use up/down keys, or resize window, to change font size. "
	info = info .. "Use left/right keys to change ascii value."
	fontKeys := list("Free/Mono", "Free/Sans", "Free/Serif", "Vera/Mono", "Vera/Sans", "Vera/Serif")
	fontMap := Map clone
	fontSize := 36
	ascii := 192
	black := Color clone set(0, 0, 0, 1)
	blue := Color clone set(0, 0, 0.5, 1)
	red := Color clone set(1, 0, 0, 1)
	norwChars := 230 asCharacter .. (248 asCharacter) .. (229 asCharacter)
)

Screen initFonts := method(
	fPath := "../../Flux/resources/fonts/"
	self infoFont = Font clone open(fPath .. "Free/Sans/Normal.ttf")
	self infoFont setPixelSize(14)
	font := nil
	fontKeys foreach(i, fk,
		//writeln("initFonts: ", fk)
		font = Font clone open(fPath .. fk .. "/Normal.ttf")
		if (false /*font error*/) then(
			writeln("*** ", fk, ": ", font error)
		) else (
			self fontMap atPut(fk, font)
		)
	)
	// Then one more for the curious ...
	if (true) then(
		font = Font clone open(fPath .. "T207/Mono/Normal.ttf")
		self fontKeys append("T207")
		self fontMap atPut("T207", font)
	) else (
		font = Font clone open("/Library/Fonts/Brush Script")
		self fontKeys append("Brush")
		self fontMap atPut("Brush", font)
	)
)
Screen initFonts

Screen setFontSize := method(size,
	self fontMap foreach(fk, font, font setPixelSize(size))
)

Screen reshape := method(width, height,
	self winHeight = height
	glViewport(0, 0, width, height)
	glLoadIdentity
	gluOrtho2D(0, width, 0, height)

	self fontSize = width / 24
	setFontSize(self fontSize)

	glutPostRedisplay
)

Screen drawShortLines := method(font, string,
	glPushMatrix
	self red glColor
	glBegin(GL_LINES)
	glVertex2d(0, 0)
	glVertex2d(font widthOfCharacter(string at(0)), 0)
	glVertex2d(0, 0)
	glVertex2d(0, font pixelSize)
	glEnd
	glPopMatrix
)

Screen drawFonts := method(color,
	fontKeys foreach(i, fk,
		font := self fontMap at(fk)
		glTranslated(0, font pixelSize * -1.2, 0)
		if (font isTextured, drawShortLines(font, fk))
		color glColor
		// The initial ">" is not supposed to get drawn,
		// as specified by the last parameter 1 (but ...)
		font drawString(">" .. fk .. " size " .. (font pixelSize asString) .. (
							", ascii " .. ascii .. ": " .. (ascii asCharacter)), 1)
		//font drawString("abc-" .. norwChars .. "-defghi", 2, 4)
	)
)

Screen display := method(
	glClearColor(1, 1, 1, 1)
	glClear(GL_COLOR_BUFFER_BIT)
	glPushMatrix
	glTranslated(300, self winHeight / 2 - 140, 0)
	glScaled(3.4, 3.4, 0)
	self red glColor
	glutStrokeString(0, ascii asCharacter)
	glPopMatrix
	
	glPushMatrix
	glTranslated(10, self winHeight, 0)
	glColor4d(0, 0.5, 0, 1)
	glTranslated(0, self infoFont pixelSize * -1, 0)
	self infoFont drawString(self info)
	
	drawFonts(self black)
	
	glScaled(0.5, 0.5, 1)
	drawFonts(self blue)
	
	glPopMatrix
	glFlush
	glutSwapBuffers
)

Screen test := method(
	fk := "Free/Mono"
	font := self fontMap at(fk)
	str := "AAAAAAAAAAAAAAA"
	w := 200
	writeln("test ", fk, " string: ", str, " - length ", str length)
	for(s, 0, str length,
		iAtW := font stringIndexAtWidth(str, s, w)
		writeln("start=", s, " width=", w, " index=", iAtW)
	)
)

Screen keyboard := method(key, x, y,
	//writeln("keyboard key=", key)
	self ascii = if(key < 128, key, 63)
	if (key == 27, test)
	self display
)

Screen special := method(c, x, y,
	if (c == GLUT_KEY_UP,
		self fontSize = self fontSize + 1
		setFontSize(self fontSize)
	)

	if (c == GLUT_KEY_DOWN,
		self fontSize = self fontSize - 1
		setFontSize(self fontSize)
	)

	if ((c == GLUT_KEY_LEFT) and (ascii > 0), ascii = ascii - 1)
	if ((c == GLUT_KEY_RIGHT) and (ascii < 255), ascii = ascii + 1)
	
	self display
)

/*
Screen mouse := method(button, state, x, y,
	if (state == 0,
		if ( y < 50,
			v := Number random(0,1)
			glColor4d(v, 0, 1 - v, 1)
		)
		self display
	)
)
*/

Screen run := method(
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(880, 540)
	glutInitWindowPosition(10, 50)
	glutInit
	glutCreateWindow("Font Test")
	glutEventTarget(Screen)
	glutDisplayFunc
	glutReshapeFunc
	glutKeyboardFunc
	glutSpecialFunc
	//glutMouseFunc

	glColor4d(0, 0, 0, 1)
	glEnable(GL_BLEND)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
	glutMainLoop
)

Screen run

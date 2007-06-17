#!/usr/bin/env io



Viewer := Object clone do(
	init := method(
		self width := 100
		self height := 100
		self isFullScreen := false
	)
	init
	
	appendProto(OpenGL)
	
	setImagePath := method(path,
		image := Image clone open(path) 
		if (image error, write("Error: ", image error, "\n"); System exit)
		self texture := image asTexture
		self windowName :=  path lastPathComponent .. " " .. image width .. " x " .. image height
		glutSetWindowTitle(windowName)
		update
	)
	
	update := method(
		if (isFullScreen, glutFullScreen, glutReshapeWindow(texture originalWidth, texture originalHeight))
		display
		//glutPostRedisplay
		Collector collect
	)
	
	reshape := method(w, h, 
		self width := w
		self height := h
		glViewport(0, 0, w, h)
		glLoadIdentity
		gluOrtho2D(0, w, 0, h)
		glutPostRedisplay
	)
	
	display := method(
		glClearColor(0,0,0, 1)
		glClear(GL_COLOR_BUFFER_BIT)
		
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		
		glPushMatrix
		
		wr := width / texture originalWidth
		hr := height / texture originalHeight
		
		if (wr < hr) then(
			glTranslated(0, (height - texture originalHeight * wr)/2, 0)
			glScaled(wr, wr, 1)
		) else(
			glTranslated((width - texture originalWidth * hr) / 2, 0, 0)
			glScaled(hr, hr, 1)
		)

		texture draw

		glPopMatrix
		
		glFlush
		glutSwapBuffers
	)
	
	special := method(key, mx, my,
		if(key == 27, isFullScreen = isFullScreen not; update)
    )
    
	run := method(
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
		glutInitWindowSize(100, 100)
		glutInit
		glutCreateWindow("")
		glutEventTarget(self)
		glutDisplayFunc
		glutReshapeFunc
		//glutKeyboardFunc
		glutSpecialFunc
		self setImagePath(Path with(launchPath, "images/lotus.png"))
		glutMainLoop
	)
)

Viewer run



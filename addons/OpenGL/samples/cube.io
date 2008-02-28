#!/usr/bin/env io


// glcube.io - A simple Openprogram
// (C) 2002 Mike Austin

// Set a simple namespace

GLCubeApp := Object clone do(
	appendProto(OpenGL)

	angleX := -26
	angleY := 74
	lastX := 0
	lastY := 0
	
	reshape := method(w, h,
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluPerspective(45, w / h, 1.0, 10.0)
		glMatrixMode(GL_MODELVIEW)
		glViewport(0, 0, w, h)
		//self width := w
		//self height := h
	)
	
	display := method(
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		
		glLoadIdentity
		glTranslated(0, 0, -3);
		glRotated(angleX, 1, 0, 0)
		glRotated(angleY, 0, 1, 0)
		glutSolidCube(1)
		
		glDisable(GL_LIGHTING)
		glColor4d(.4,.4,.4, 1)
		glutWireCube(1.002)
		glEnable(GL_LIGHTING)
		
		glFlush
		glutSwapBuffers
	)

	mouse := method(button, state, x, y,
		lastX = x
		lastY = y
		
	)

	motion := method(x, y,
		angleX = angleX + (y - lastY)
		angleY = angleY + (x - lastX)
		lastX = x
		lastY = y
		glutPostRedisplay
	)

    keyboard := method(key, mx, my,
            if(key asCharacter == " ",
				data := Sequence clone 
				width := glutGet(GLUT_WINDOW_WIDTH)
				height := glutGet(GLUT_WINDOW_HEIGHT)
				//GLUT_WINDOW_ALPHA_SIZE
				glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data)
				image := Image clone 
				image setDataWidthHeightComponentCount(data, width, height, 4)
				image flipY
				image save("screenshot.jpg")
				writeln("saved screenshot.jpg")
			)
            display
    )

	run := method(
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)
		glutInitWindowSize(512, 512)
		glutInit
		glutCreateWindow("Io Cube")
		glutEventTarget(self)
		glutReshapeFunc
		glutDisplayFunc
		glutMouseFunc
		glutMotionFunc
		glutKeyboardFunc
		
		glClearColor(1, 1, 1, 1)
		glEnable(GL_DEPTH_TEST)
		glEnable(GL_LIGHTING)
		glEnable(GL_LIGHT0)
		glDisable(GL_CULL_FACE)
		
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glEnable(GL_LINE_SMOOTH)
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
		glLineWidth(2)
		
		glutMainLoop
	)
)

GLCubeApp clone run

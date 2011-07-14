#!/usr/bin/env io

GLSphereApp := Object clone do(
	appendProto(OpenGL)

	angleX := -45
	angleY := 0
	lastX := 0
	lastY := 0
	
	reshape := method(w, h,
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluPerspective(45, w / h, 1.0, 10.0)
		glMatrixMode(GL_MODELVIEW)
		glViewport(0, 0, w, h)
	)

	display := method(
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		
		glLoadIdentity
		glTranslated(0, 0, -3)
		glRotated(angleX, 1, 0, 0)
		glRotated(angleY, 0, 1, 0)
		sphere useFillStyle
		sphere draw

		glDisable(GL_LIGHTING)
		glColor4d(.4,.4,.4, 1)
		sphere useLineStyle
		sphere draw
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

	run := method(
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)
		glutInitWindowSize(512, 512)
		glutInit; glutCreateWindow("Io Sphere")
		glutEventTarget(self)
		glutReshapeFunc
		glutDisplayFunc
		glutMouseFunc
		glutMotionFunc
		
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

		self sphere := GLUSphere clone do(
			setRadius(0.8)
			setSlices(90)
			setStacks(90)
		)
		
		glutMainLoop
	)
)

GLSphereApp clone run

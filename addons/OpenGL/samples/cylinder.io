#!/usr/bin/env io

GLCylinderApp := Object clone do(
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
		glTranslated(0, 0, -5)
		glRotated(angleX, 1, 0, 0)
		glRotated(angleY, 0, 1, 0)

		glTranslated(0, 0, -1.5)

		cylinder useFillStyle
		cylinder draw

		glDisable(GL_LIGHTING)
		glColor4d(.4,.4,.4, 1)
		cylinder useLineStyle
		cylinder draw
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
		glutInit; glutCreateWindow("Io Cylinder")
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
		
		self cylinder := GLUCylinder clone do(
			setBaseRadius(1.0)
			setTopRadius(1.0)
			setHeight(3.0)
			setSlices(16)
			setStacks(8)
		)
	
		glutMainLoop
	)
)

GLCylinderApp clone run

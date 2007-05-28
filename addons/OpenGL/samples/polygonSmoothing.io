#!/usr/bin/env io

// A program to test OpenGL events
// Steve Dekorte, 2002

Screen := Object clone do(
	appendProto(OpenGL)
	style := 0
	timersize := 0
	
	reshape := method(w, h, 
		self width := w
		self height := h
		glViewport(0,0,w,h)
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluOrtho2D(0, w, 0, h)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity
		glClearColor(0,0,0, 1)
		display
	)
	
	display := method(
		glClear(GL_COLOR_BUFFER_BIT)
		glLoadIdentity
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glColor4d(1, 1, 1, .7)
		self perform("draw" .. style)
		glFlush
		glutSwapBuffers
	)

	jitter2 := List clone append(
		vector(.25, .75),
		vector(.75, .25)
	)
		
	jitter2n := List clone append(
		vector(1, 1),
		vector(1, -1)
		vector(-1, 1)
		vector(-1, -1)
	)
	
	jitter4 := List clone append(
		vector(.375, .25),
		vector(.125, .75),
		vector(.875, .25),
		vector(.625, .75)
	)
	
	jitter := jitter2n
		
	show := method(s,
		glTranslated(0, 10, 0)
		glScaled(.2,.2, 1)
		glutStrokeString(GLUT_STROKE_ROMAN, s)
	)
	
	draw0 := method(
		glColor4d(1, 1, 1, 1)
		glDisable(GL_POLYGON_SMOOTH)
		drawPolygon
		show("no smoothing")
	)
	
	draw2 := method(
		glColor4d(1, 1, 1, 1)
		glEnable(GL_POLYGON_SMOOTH)
		glDisable(GL_DEPTH_TEST)
		//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE)
		drawPolygon
		glDisable(GL_POLYGON_SMOOTH)
		show("GL_POLYGON_SMOOTH")
	)
	
	draw1 := method(
	   writeln("draw1")
		glColor4d(1, 1, 1, 1)
		drawPolygon(.2)
		//glColor4d(1, 1, 1, 1/(jitter size))
		//glColor4d(1, 1, 1, 1/(jitter size))
		glColor4d(1, 1, 1, .5)
		//glDisable(GL_POLYGON_SMOOTH)
		glEnable(GL_POLYGON_SMOOTH)
		jitter foreach(j, 
			glPushMatrix 
			glTranslated(j x * .25, j y * .25, 0)
			drawPolygon
			glPopMatrix
		)	
		glColor4d(1, 1, 1, 1)
		show("jitter with alpha blending")
	)

	draw3 := method(
		glDisable(GL_POLYGON_SMOOTH)
		glClear(GL_ACCUM_BUFFER_BIT)
		jitter4 foreach(j, 
			glClear(GL_COLOR_BUFFER_BIT)
			glPushMatrix 
			glTranslated(j x, j y, 0)
			self drawPolygon
			glPopMatrix
			glAccum(GL_ACCUM, 1.0/jitter4 size)
		)
		glAccum(GL_RETURN, 1.0)
		glFlush
		show("jitter + accumulation")
	)
	
	drawPolygon := method(a,
		if(a == nil, a = 0)
		glPushMatrix
		glTranslated(width/2, height/2, 0)
		glRotated(-timersize, 0, 0, 1)
		gluPartialDisk(q, 100+a, 300-a, 32, 1, 0, 180) 
		glPopMatrix
	)
	
	q := method(self q := gluNewQuadric)
	
	mouse := method(button, state, x, y,
		if (state == 1, style = style + 1)
		if (style > 1, style = 0)
		//write("mouse style = ", style, "\n")
		glutPostRedisplay
	)
	
	timer := method(v,
		timersize =  timersize + .1
		if (timersize > 360, timersize := 0)
		glutTimerFunc(500/60, 0)
		glutPostRedisplay
	)
	
	run := method(
		glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_ACCUM | GLUT_ALPHA )
		glutInitWindowSize(800, 800)
		glutInit
		glutCreateWindow("Polygon Smoothing - press mouse button")
		glutReshapeWindow(800, 800)
		glutEventTarget(self)
		glutDisplayFunc
		glutMouseFunc
		glutReshapeFunc
		glutTimerFunc(100, 0)
		
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glutMainLoop
	)
) run



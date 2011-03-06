#!/usr/bin/env io

GLSphereApp := Object clone do(
	appendProto(OpenGL)

	angleX := 90
	angleY := 0
	angleZ := 150
	lastX := 0
	lastY := 0
	zSpeed := 1
	minSpeed := 0.01
	maxSpeed := 10
	
	reshape := method(w, h,
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluPerspective(45, w / h, 1.0, 10.0)
		glMatrixMode(GL_MODELVIEW)
		glViewport(0, 0, w, h)
	)

    didSetup := false
    setup := method(
        self texture := Texture with(Image clone setPath("./images/world_hires2.jpg") open)
        //self texture := Texture with(Image clone setPath("./images/earthnight.jpg") open)
		sphere setTexture(texture)
		didSetup = true
    )
    
	display := method(
		if(didSetup not, setup)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		
		// glBlendFunc( GL_SRC_ALPHA_SATURATE, GL_ONE );
		glEnable( GL_BLEND );
		glEnable( GL_POLYGON_SMOOTH );
		glDisable( GL_DEPTH_TEST );
   
		glLoadIdentity
		glTranslated(1, 0, -2.3)
		glRotated(angleX, 1, 0, 0)
		glRotated(angleY, 0, 1, 0)
		glRotated(angleZ, 0, 0, 1)
		
        sphere setRadius(0.8)
		sphere useFillStyle
		//sphere draw
		
		sphere texture bind
        glColor4d(0,0,0,0)	
		gluSphere(sphere, sphere radius, sphere slices, sphere stacks)

        
   //glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE);
        glDisable(GL_TEXTURE_2D);	
		glDisable(GL_LIGHTING)
		for(r, .81, .84, .002,
            //atmosphere1 useLineStyle
            glRotated(.01, 0, 1, 0)
            glRotated(.01, 0, 0, 1)
            glColor4d(.8, .8, 1, .005)	
            atmosphere1 setRadius(r)
            atmosphere1 draw
		)
  		glEnable(GL_LIGHTING)
        glEnable(GL_TEXTURE_2D);
        			
        
		glFlush
		glutSwapBuffers
	)

	mouse := method(button, state, x, y,
		lastX = x
		lastY = y
	)

	motion := method(x, y,
		//angleX = angleX + (y - lastY)
		//angleZ = angleZ - (x - lastX)
		zSpeed = (x - lastX)*.3
		lastX = x
		lastY = y
		glutPostRedisplay
	)
	
    timer := method(v,
        glutTimerFunc(10, 0)
        angleZ =  angleZ - zSpeed
        zSpeed = zSpeed * .98 //- (zSpeed*zSpeed) log)
        if(zSpeed < minSpeed, zSpeed = minSpeed)
        if(zSpeed > maxSpeed, zSpeed = maxSpeed)
        self glutPostRedisplay
    )

	run := method(
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)
		glutInitWindowSize(800, 1150)
		glutInit
		glutCreateWindow("earth")
		glutEventTarget(self)
		glutReshapeFunc
		glutDisplayFunc
		glutMouseFunc
		glutMotionFunc
        glutTimerFunc(10, 0)
		
		glClearColor(0, 0, 0, 1)
		glEnable(GL_DEPTH_TEST)
		glDisable(GL_DEPTH_TEST)
		glEnable(GL_LIGHTING)
		glEnable(GL_LIGHT0)
		glEnable(GL_CULL_FACE)
		
		glEnable(GL_BLEND)
		//glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		//glBlendFunc (GL_ONE, GL_ONE);
         glEnable (GL_POLYGON_SMOOTH);
		//glEnable(GL_LINE_SMOOTH)
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
		//glLineWidth(2)
        glEnable(GL_TEXTURE_2D);	


		self sphere := GLUSphere clone do(
			setRadius(0.8)
			setSlices(128)
			setStacks(128)
		)
		sphere useFillStyle
		sphere useSmoothNormals
		sphere useOutsideOrientation
		//sphere useInsideOrientation
  
		self atmosphere1 := GLUSphere clone do(
			setRadius(0.83)
			setSlices(256)
			setStacks(256)
		)
		atmosphere1 useFillStyle
		atmosphere1 useSmoothNormals
		atmosphere1 useOutsideOrientation
		  
		glutMainLoop
	)
)

GLSphereApp clone run

# Port of http://pyode.sourceforge.net/tutorials/tutorial3.html

appendProto(OpenGL)

scalp := method(vec, scal,
	vec *= scal
)

length := method(vec,
	vec rootMeanSquare
)

drawBody := method(body,
	pos := body position
	r := body rotation
	rot := list(r at(0), r at(3), r at(6), 0, r at(1), r at(4), r at(7), 0, r at(2), r at(5), r at(8), 0, pos x, pos y, pos z, 1)
	glPushMatrix()
	glMultMatrixd(rot)
	if(body shape == "box",
		body size glScale
		glColor4d(0,0,1, 1)
		glutSolidCube(1)
		glEnable(GL_BLEND)

        glDisable(GL_LIGHTING)
		glColor4d(.4,.4,.4, 1)
		glutWireCube(1.002)
		glEnable(GL_LIGHTING)
	)
	glPopMatrix
)

geoms := list # work around for bug which means geoms were gc'ed too early
createBox := method(world, space, density, lx, ly, lz,
	# Create body
	body := world Body clone
	body setMass(ODEMass clone setBoxDensity(density, lx, ly, lz))

	# Set parameters for drawing the body
	body shape := "box"
	body size := vector(lx, ly, lz)

	# Create a box geom for collision detection
	geom := space box(lx, ly, lz) setBody(body)
	geoms append(geom)

	body
)

dropObject := method(
	body := createBox(world, space, 2000, 9/9, 1/9, 4/9)
	body setPosition(Random gaussian(0, 0.5), 3.0, Random gaussian(0, 0.5))

	theta := Random value * 2 * Number constants pi * 2
	ct := theta cos
	st := theta sin
	body setRotation(ct, 0, -st, 0, 1, 0, st, 0, ct)

	bodies append(body)
	counter = 0
	objCount = objCount + 1
)

origin := vector(0,0,0)

explosion := method(
	bodies foreach(b,
		l := b position 
		l setY(1)
		d := l distanceTo(origin)
		force := 300000 / (1 + d)
		l normalize *= force
		b addForce(l x, l y, l z)
	)
)

bump := method(
	bodies foreach(b,
		b addForce(0,20000,0)
	)
)

implode := method(
	bodies foreach(b,
		l := b position clone
		d := l distanceTo(origin)
		force := -45000 * d sqrt
		l normalize *= force
		b addForce(l x, l y, l z)
	)
)

nearCallback := method(geom1, geom2,
	contacts := geom1 collide(geom2, 8) # find a max of 8 contacts
	contacts foreach(contact,
		contact setBounce(0.2) setMu(5000)
		contactgroup createContact(contact) attach(geom1 body, geom2 body)
	)
)


######################################################################

# Create a world object
world := ODEWorld clone setGravity(0, -9.81, 0) 
world setErp(0.8) 
world setCfm(1E-5)

# Create a space object
space := ODESimpleSpace clone

# Create a plane geom which prevent the objects from falling forever
floor := space plane(0, 1, 0, 0)

# A list with ODE bodies
bodies := list

# A joint group for the contact joints that are generated whenever
# two bodies collide
contactgroup := world JointGroup clone

# Some variables used inside the simulation loop

# Glut events object
Screen := Object clone do(
	newSlot("width", 640)
	newSlot("height", 480)

	newSlot("state", 0)
	newSlot("counter", 0)
	newSlot("objCount", 0)
	
	reshape := method( w, h,
		setWidth(w) setHeight(h)
        glMatrixMode(GL_PROJECTION)
		glLoadIdentity
		gluPerspective(45, w / h, 1.0, 20.0)
		glMatrixMode(GL_MODELVIEW)
		glViewport(0, 0, w, h)
	)

	display := method(
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		glLoadIdentity
	    gluLookAt (2.4, 3.6, 4.8, 0.5, 0.5, 0, 0, 1, 0)
	    glPushMatrix
	    glScaled(1,0,1)
        //glutSolidCube(10)
        glDisable(GL_LIGHTING)
        glLineWidth(1)
        glColor4d(1,1,1,.4)
	    for(x, -10, 10,
	    for(z, -10, 10,
            glPushMatrix
            glTranslated(x,0,z)
            glutWireCube(1)
            glPopMatrix
        ))
        glEnable(GL_LIGHTING)
        glPopMatrix
		bodies foreach(b, drawBody(b))
		glFlush
		glutSwapBuffers
	)

	timer := method(arg,
		counter = counter + 1

		if(state == 0,
			if(counter == 50, dropObject)
			if(objCount == 20, state = 1; counter = 0)
		,
			if(counter == 100, explosion)
			if(counter > 400,  if(counter % 20 == 0, bump; implode))
			if(counter == 600, counter = 0)
		)

		glutPostRedisplay

		n := 2

		# Simulate
		n repeat(
			# Detect collisions and create contact joints
			space collide(Lobby, message(nearCallback))

			# Simulation step
			world step(arg / 1200 / n)

			# Remove all contact joints
			contactgroup empty
		)

		glutTimerFunc( arg, arg )
	)
)

glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH)
glutInitWindowSize( Screen width, Screen height )
glutInit
glutCreateWindow("ODE Example")
glutEventTarget( Screen )
glutReshapeFunc
glutDisplayFunc
glutTimerFunc( 0, 10 )

glClearColor(1,1,1, 1 )
glClearColor(0,0,0, 1 )
glEnable( GL_DEPTH_TEST )
glEnable( GL_LIGHTING )
glEnable( GL_LIGHT0 )
glEnable(GL_DEPTH_TEST)
glEnable(GL_NORMALIZE)
glShadeModel(GL_SMOOTH)
glDisable(GL_CULL_FACE)

glEnable(GL_LINE_SMOOTH)
glEnable(GL_BLEND)
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)

glLightfv(GL_LIGHT0,GL_POSITION, list(0,0,1,0))
glLightfv(GL_LIGHT0,GL_DIFFUSE, list(1,1,1,1))
glLightfv(GL_LIGHT0,GL_SPECULAR, list(1,1,1,1))
glEnable(GL_LIGHT0)

# View transformation
Coroutine setStackSize(1024*1024)
e := try(
    glutMainLoop
)
if(e, e showStack)

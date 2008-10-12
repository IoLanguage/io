#!/usr/bin/env io

# game.io - A simple ship flying around a window
# (C) 2002 Mike Austin
# Updated by Steve Dekorte
# Significant cleanup and simplification by Jonathan Wright - Oct 2006


appendProto(OpenGL)

Ship := Object clone do(
    newSlot("location")
    newSlot("velocity")
    newSlot("impulse")
    newSlot("angle", 180)

    newSlot("thrust", 0.05)

    newSlot("turningLeft", false)
    newSlot("turningRight", false)
    newSlot("thrusting", false)

    newSlot("screenSize")

    init := method(
        # Reuse the vectors so we don't put presure on the garbage collector
        location = vector(0, 0, 0)
        velocity = vector(0, 0, 0)
        impulse = vector(0, 0, 0)
    )

	draw := method(
		glPushMatrix

        location glTranslate
		glRotated(angle, 0, 0, 1)

		glColor4d(1, 1, 1, 1)
		glScaled(5, 5, 0)

        # nice anti-aliased outline with rounded corners due to the
        # glEnable(GL_LINE_SMOOTH) and glLineWidth(2)
        glBegin(GL_LINE_LOOP)
		glVertex3d(0,-3, 0)
		glVertex3d(-2, 2, 0)
		glVertex3d(2, 2, 0)
		glEnd

        # fill in the outline
        glBegin(GL_POLYGON)
		glVertex3d(0,-3, 0)
		glVertex3d(-2, 2, 0)
		glVertex3d(2, 2, 0)
		glEnd

		glPopMatrix
	)

	update := method(
        if(turningLeft,
            angle = angle + 5
        )

        if(turningRight,
            angle = angle - 5
        )

        angle = if(angle < 0, angle + 360, angle % 360)

        if(thrusting,
            a := angle * (Number constants pi / 180)
            velocity += impulse set(a sin * thrust, a cos * -thrust, 0)
        )

        location += velocity

        wrapLocation
    )

    centerOnScreen := method(
        location = screenSize / 2
    )

    wrapLocation := method(
        if (location x > screenSize x, location setX(0))
        if (location x < 0, location setX(screenSize x))

        if (location y > screenSize y, location setY(0))
        if (location y < 0, location setY(screenSize y))
	)
)

Screen := Object clone do(
    newSlot("ship")

    newSlot("size") # set by the reshape callback
    newSlot("scale")

    init := method(
        size = vector(0, 0)
        ship = Ship clone setScreenSize(size)
    )

    width := method(size x)
    height := method(size y)

    //doc Screen reshape  Reshape callback for glut.
    reshape := method(w, h,
        size set(w, h)
        scale = vector(2,2) / size

        ship centerOnScreen

        glViewport(0, 0, w, h)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity
        gluOrtho2D(-1, -1, 1, 1)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity
    )

    //doc Screen display Display callback for glut.
    display := method(
        glClear(GL_COLOR_BUFFER_BIT)
        glLoadIdentity
        glTranslated(-1, -1, 0)
        scale glScale

        ship draw

        glFlush
        glutSwapBuffers
    )

    //doc Screen timer Timer callback for glut.
    timer := method(v,
        glutTimerFunc(10, 0)

        ship update

        self glutPostRedisplay
    )

    //doc Screen keyboard Key down callback for glut.
    keyboard := method(key, x, y,
        if(key asCharacter == "4", ship setTurningLeft(true))
        if(key asCharacter == "6", ship setTurningRight(true))
        if(key asCharacter == "8", ship setThrusting(true))
    )

    //doc Screen keyboardUp Key up callback for glut.
    keyboardUp := method(key, x, y,
        if(key asCharacter == "4", ship setTurningLeft(false))
        if(key asCharacter == "6", ship setTurningRight(false))
        if(key asCharacter == "8", ship setThrusting(false))
    )
)

//doc run Sets up GLUT, creates the window and sets a few of the opengl defaults.
run := method(
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(200, 100)
    glutInitWindowSize(512, 512)
    glutInit
    glutCreateWindow("Io Ship")

    glutEventTarget(Screen clone)
    glutReshapeFunc
    glutDisplayFunc
    glutKeyboardFunc
    glutKeyboardUpFunc
    glutTimerFunc(0, 0)

    glClearColor(0, 0, 0, 1)
    glutIgnoreKeyRepeat(1)

    glEnable(GL_BLEND)
    glEnable(GL_LINE_SMOOTH)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    glLineWidth(2)

    glutMainLoop
)

writeln("Controls\n  forward  keypad 8\n  left     keypad 4\n  right    keypad 6")
run

# vim:ts=4:sts=4:sw=4:et:

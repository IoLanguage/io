#!/usr/bin/env io

// A program to test OpenGL events
// Steve Dekorte, 2002

Screen := Object clone
Screen appendProto(OpenGL)

Screen do(
        timerCount := 0
        width := 0
        height := 0
        y := 0
        x := 0
        jb := 0
        jv := vector(0, 0, 0)
        xDown := nil
        yDown := nil
        action := "move mouse to start"

        reshape := method(w, h, 
                width = w
                height = h
                glViewport(0,0,w,h)
                glMatrixMode(GL_PROJECTION)
                glLoadIdentity
                gluOrtho2D(0, w, 0, h)
                glMatrixMode(GL_MODELVIEW)
                glLoadIdentity
                glClearColor(1, 1, 1, 1)
                display
        )

        display := method(
                glClear(GL_COLOR_BUFFER_BIT)
                glLoadIdentity

                if (xDown, drawSelection)
                drawCrossHairs
                drawActionMessage
                drawTimerMessage

                glFlush
                glutSwapBuffers
        )

        drawCrossHairs := method(
                glPushMatrix
                glColor4d(.7, .7, .7, 1)
                glBegin(GL_LINES)
                glVertex3d(x, 0, 0)
                glVertex3d(x, height, 0)
                glVertex3d(0, y, 0)
                glVertex3d(width, y, 0)
                glEnd
                glPopMatrix
        )

        drawSelection := method(
                glColor4d(.7, .7, .7, .1)
                glBegin(GL_POLYGON)
                glVertex3d(xDown, yDown, 0)
                glVertex3d(xDown, y, 0)
                glVertex3d(x, y, 0)
                glVertex3d(x, yDown, 0)
                glEnd

                glColor4d(0,0,0, .1)
                glBegin(GL_LINE_LOOP)
                glVertex3d(xDown, yDown, 0)
                glVertex3d(xDown, y, 0)
                glVertex3d(x, y, 0)
                glVertex3d(x, yDown, 0)
                glEnd

                b := 5
                glColor4d(1, 0, 0, 1)
                glRectd(x - b, y - b, x + b, y + b)
        )

        drawActionMessage := method(
                glColor4d(.5, .5, .5, 1)
                glColor4d(0,0,0, 1)
                glPushMatrix
                glTranslated(10, 10, 0)
                drawString(action)
                glPopMatrix
        )

        drawString := method(string,
                if (self ?font,
                        nil //font drawString(string)
                ,
                        glPushMatrix
                        glScaled(.1, .1, 0)
                        glutStrokeString(0, string)
                        glPopMatrix
                )
        )

        drawTimerMessage := method(
                glColor4d(.5, .5, .5, 1)
                glColor4d(0,0,0, 1)
                glPushMatrix
                glTranslated(10, 30, 0)
                drawString("timer ".. timerCount)
                glPopMatrix
                glPushMatrix
                glTranslated(10, 50, 0)
                drawString("joystick ".. jb .. " " .. jv)
                glPopMatrix
        )


        keyboard := method(key, mx, my,
                action = "keyboard(" .. key asCharacter ..", " .. mx floor .. ", ".. my floor .. ")"
                x = mx; y = height - my
                display
        )

        special := method(key, mx, my,
                action = "special(" .. key ..", " .. mx floor .. ", ".. my floor .. ")"
                x = mx; y = height - my
                display
        )

        motion := method(mx, my,
                action = "motion(" .. mx floor .. ", ".. my floor .. ")"
                x = mx; y = height - my
                display
        )

        passiveMotion := method(mx, my,
                action = "passiveMotion(" .. mx floor .. ", " .. my floor .. ")"
                x = mx; y = height - my
                display
        )

        mouse := method(button, state, mx, my,
                action = "mouse(" .. button floor ..", " .. state floor .. ", " .. mx floor .. ", " .. my floor .. ")"
                x = mx; y = height - my

                if (state == 0,
                        xDown = x
                        yDown = y
                ,
                        xDown = nil
                        yDown = nil
                )

                action = action .. " modifiers: " .. glutGetModifiers 

                display
        )

        timer := method(v,
                timerCount = timerCount + 1
                glutTimerFunc(100, 0)
                display
        )

        joystick := method(b, v,
                jb = b
                jv = v
        )


        run := method(
                glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA )
                glutInitWindowSize(512, 512)
                glutInit
                glutCreateWindow("Events Test")
                glutEventTarget(self)

                # register event functions
                glutDisplayFunc
                glutKeyboardFunc
                glutSpecialFunc
                glutMotionFunc
                glutMouseFunc
                glutReshapeFunc
                glutPassiveMotionFunc
                glutMouseFunc
                //glutInitJoystick("/dev/input/js0")
                glutJoystickFunc(100)
                glutTimerFunc(100, 0)
                glutReshapeWindow(1000,1000)

                glEnable(GL_LINE_SMOOTH)
                glEnable(GL_BLEND)
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
                //glBlendFunc(GL_SRC_ALPHA, GL_ONE)
                glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
                glLineWidth(1)
                glutMainLoop
        )
)

Screen run


#!/usr/bin/env io


Screen := Object clone
Screen appendProto(OpenGL)

Screen reshape := method(w, h, 
        self width := w
        self height := h
        glViewport(0,0,w,h)
        gluOrtho2D(0,0,w,h)
        glClear(GL_COLOR_BUFFER_BIT)
        glutPostRedisplay
)

Screen image := nil
Screen grab := method(
        if (image, return)
        write("grabbing\n")
        self image := Image clone
        draw
        glFlush
        glutSwapBuffers
        glReadBuffer(GL_BACK)
        image grabTexture(0, 0, 64, 64)
        write("image ", image width, "x", image height, "\n")
)

Screen size := 0
Screen draw := method(
        //write("draw ", size % 2 , "\n\n")
        //size ++
        glColor4d(1,0,0,1)
        Point clone set(400,100) drawQuad
)

Screen display := method(
        glClearColor(1, 1, 1, 1)
        glClear(GL_COLOR_BUFFER_BIT)

        glMatrixMode(GL_PROJECTION)
        glLoadIdentity
        gluOrtho2D(0, width, 0, height)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity

        write("display\n")

        grab
        glPushMatrix
        glTranslated(200,200, 0)
        image drawTexture
        glPopMatrix
        //draw

        glFlush
        glutSwapBuffers
)

Screen timer := method(
        glutTimerFunc(1000, 0)
        display
)

Screen run := method(
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
        glutInitWindowSize(512, 512)
        glutInit
        glutCreateWindow("lines")
        glutEventTarget(Screen)
        glutDisplayFunc
        glutReshapeFunc
        //glutTimerFunc(100, 0)

        glEnable(GL_TEXTURE_2D)
        //glEnable(GL_BLEND)
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glutMainLoop
)

Screen run



BusyView := View clone do(
    counter := 0
    isActive ::= false
	q := method(self q := gluNewQuadric)
	position setX(0) setY(0)
	
	draw := method(
        isActive ifFalse(return)
		glPushMatrix
		glTranslated(width/2, height/2, 0)
		glRotated(-counter * 20, 0, 0, 1)
		d := 2
		r := 6
		max := 10
		glLineWidth(1)
		for(i, 0, max,
		    c := .7*(i/max)
            glColor4d(c,c,c, 1)
            gluQuadricDrawStyle(q, GLU_FILL)
            gluPartialDisk(q, r/2, r, d, 1,  i*360/max, 360/max) 
            gluQuadricDrawStyle(q, GLU_LINE)
            gluPartialDisk(q, r/2, r, d, 1,  i*360/max, 360/max) 
		)
		glEnd
		glPopMatrix
	)
    
    start := method(
        setIsActive(true)
        timer
    )
    
    stop := method(
        setIsActive(false)
        topWindow ?removeTimerWithTarget(self)
    )
    
	timer := method(v,
		counter =  counter + 1
		if (counter > 360, counter := 0)
		//writeln("BusyView timer ", topWindow type)
		topWindow ?addTimerTargetWithDelay(self, .1)
		glutPostRedisplay
	)
)

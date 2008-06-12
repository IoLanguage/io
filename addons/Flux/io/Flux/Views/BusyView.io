
BusyView := View clone do(
    counter := 0
    isActive ::= false
	q := method(self q := gluNewQuadric)
	position setX(0) setY(0)
	dlist := method(
        writeln(self uniqueId, " dlist begin")
        d := 2
        r := 6
        max := 10
        glLineWidth(1)
        dlist = DisplayList clone
        dlist begin
        for(i, 0, max,
            c := .5 * i/max
            glColor4d(c,c,c, 1)
            gluQuadricDrawStyle(q, GLU_FILL)
            gluPartialDisk(q, r/2, r, d, 1,  i*360/max, 360/max) 
            gluQuadricDrawStyle(q, GLU_LINE)
            gluPartialDisk(q, r/2, r, d, 1,  i*360/max, 360/max) 
        )
        dlist end
        dlist
	)
	
	draw := method(
       	isActive ifFalse(return)
 		//writeln("BusyView draw ", position, " ", size)
		glColor4d(1,1,1,1)
		
		glPushMatrix
		glTranslated(width/2, height/2, 0)
		glRotated(-counter * 20, 0, 0, 1)
		dlist call
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
		//writeln("BusyView timer")
		topWindow ?addTimerTargetWithDelay(self, .07)
		setNeedsRedraw(true)
		yield
		glutPostRedisplay
	)
)

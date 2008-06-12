
SheetView := View clone do(
    idCount := 0
    uniqueId := idCount
    
    init := method(
		resend
		SheetView idCount = SheetView idCount + 1
		self uniqueId := idCount
    )
    
    font := fonts system bold
    
    borderSize ::= 50
    
    setResizeWidth(101)
    setResizeHeight(101)
    
    title ::= "1"
    previousSheet ::= nil
    nextSheet ::= nil
    
    motionAction := nil
    
    textColor ::= Color clone set(1, 1, 1, 1)
    backgroundColor ::= Color clone set(.5, .5, .5, .5)
    targetPosition ::= nil
    targetSize ::= nil
    
    draw := method(
		drawBackground
		// draw label
		glPushMatrix
		textColor glColor
		fontWidth := font widthOfString(title)
		fontHeight := 32 //font pixelHeight 
		
		b := ((width - fontWidth) * .5) round
		glTranslatei(b, (height - fontHeight)/2, 0)
		font drawString(title)
		glPopMatrix
    )

    drawBackground := method(
    	backgroundColor glColor
		glRectd(0, 0, size width, size height)
    )
    
    leftMouseMotion := method(
		writeln("leftMouseMotion")
		//moveWithMouseMotion
    )

    rightOriginPoint  := method(Point clone setX(topWindow size width))
    centerOriginPoint := method(Point clone)
    leftOriginPoint   := method(Point clone setX(topWindow size width negate))

    inTopWindow := method(if(topWindow subviews contains(self), "(in top)", "(not in top)"))
    
    nil uniqueId := "nil"
    
    print := method(
		writeln("SheetView ", self uniqueId)
		if (nextSheet, writeln("  nextSheet ", nextSheet uniqueId, " ", nextSheet inTopWindow))
		if (previousSheet, writeln("  previousSheet ", previousSheet uniqueId, " ", previousSheet inTopWindow))
		writeln("  self ", self inTopWindow)
    )
    
    leftMouseDown := method(
		//writeln(self uniqueId, " leftMouseDown")
		//moveWithMouseMotion
		setNextSheet(SheetView clone setPreviousSheet(self) autoSize)
		writeln(self uniqueId, ": nextSheet ", nextSheet uniqueId)
		nextSheet setPreviousSheet(self)
		nextSheet setTitle((title asNumber + 1) asString)
		nextSheet size copy(topWindow size)
		nextSheet setPosition(rightOriginPoint)
		nextSheet setTargetPosition(centerOriginPoint)
		topWindow addSubview(nextSheet)
		writeln(topWindow uniqueId, " addSubview(", nextSheet uniqueId, ")")
		
		topWindow uniqueId := "topWindow"
		self print
			
		writeln("--------")
		topWindow removeSubview(self)
		self setTargetPosition(leftOriginPoint/2)
		self startAnimating
		nextSheet startAnimating
		self topWindow := topWindow
		nextSheet topWindow := topWindow
    )
    
    drawOutline := nil

    autoSize := method(
		size copy(topWindow size)
		self
    )
    
    positionCenter := method(
		position setX(0)
		position setY(0)
		self
    )
    
    startAnimating := method(
		topWindow addTimerTargetWithDelay(self, 1/60)
    )

    stopAnimating := method(
		topWindow removeTimerWithTarget(self)
    )
    
    stepTowardTargetPosition := method(
		position setSize(2)
		p := targetPosition - position 
		p *= .02
		position += p
    )
    
    timer := method(
		//writeln(self uniqueId, " timer")
		stepTowardTargetPosition
		//writeln("position = ", position - targetPosition)
		//writeln("d = ",position distanceTo(targetPosition))
		if(position distanceTo(targetPosition) < .1) then(
			position copy(targetPosition)
			setTargetPosition(nil)
			//stopAnimating
			
			if(previousSheet, 
				writeln(self uniqueId, ": ", topWindow uniqueId, " removeSubview(", previousSheet uniqueId, ")\n")
				
				/*
				if(topWindow subviews contains(previousSheet),
					Exception raise("SheetView", "previousSheet not removed")
				)
				*/
				topWindow removeSubview(previousSheet)
				self setPreviousSheet(nil)
				//writeln("previousSheet superview = ", previousSheet superview)
				
				previousSheet leftMouseDown := method(
					Error raise("SheetView", "this sheet should be dead!")
					System exit
				)
				
			)
		) else(
			topWindow addTimerTargetWithDelay(self, 1/100)
		)
		glutPostRedisplay
    )
)


TextField := View clone do(
    isSelected := 0
    cursorPosition := 0
    endCursorPosition := nil
    size setWidth(100) setHeight(24)
    textColor := Color clone set(0, 0, 0, 1)
    selectionColor := Color clone set(0, 0, 0, .3)
    backgroundColor := Color clone set(.5,.5,.5,1)
    textures := TextureGroup clone loadGroupNamed("TextField/Round")
    font := fonts system normal
    
    string := Sequence clone
    setString := method(s, string = s clone asBuffer)
    setString("")
    setAcceptsFirstResponder(true)

    init := method(
		resend
		cursorPosition = cursorPosition
		endCursorPosition = nil
		self keyActions := Map clone
		self string := string clone
    )

    forKeyAddAction := method(key, action,
		keyActions atPut(key, action)
		action
    )
    
    actionForKey := method(key, keyActions at(key))
    
    cursorX := method(
		font widthOfString(renderString, 0, cursorPosition)
    )

    cursorX2 := method(
		font widthOfString(renderString, 0, endCursorPosition)
    )
        
    textXOffset := method(
		4 + textures left width / 2
    )
    
    renderString := method(string)

    draw := method(
		if(isTextured) then(
            textures draw(width, 32)
		) else(
            backgroundColor glColor
            drawRoundedBox
		)
		
		glTranslated(textXOffset, 0, 0)
		h := height - 4
		if(isFirstResponder, 
			x1 := cursorX
			glPushMatrix
			if(endCursorPosition) then(
				x2 := cursorX2 
				selectionColor glColor
				glBegin(GL_QUADS)
				glVertex2i(x1, 1)
				glVertex2i(x1, h)
				glVertex2i(x2, h)
				glVertex2i(x2, 1)
				glEnd	    
			) else(
				textColor glColor
				glBegin(GL_LINES)
				glVertex2i(x1, 1)
				glVertex2i(x1, h)
				glEnd
			)
			glPopMatrix
		)
		
		glTranslatei(0, (3 + size y - font pixelHeight) / 2, 0)
		textColor glColor
		font drawString(renderString)
    )
    
    //drawBackground := nil
    
    mouseCursorPosition := method(
		x := viewMousePoint x - textXOffset
		p := font stringIndexAtWidth(renderString, 0, x)
		p = p max(0) min(renderString size)
		p
    )
    
    leftMouseDown := method(
		cursorPosition = mouseCursorPosition	
		//delegate ?textFieldClick(self)) 
		isSelected = if(Mouse state == 1, 0, 1)
		endCursorPosition = nil
		makeFirstResponder
    )

    doubleMouseDown := method(
		makeFirstResponder
    )

    doubleMouseUp := method(
		makeFirstResponder
    )

    leftMouseMotion := method(
		endCursorPosition = mouseCursorPosition
		glutPostRedisplay
    )
    
    keyboard := method(key, x, y,
		// del = 8, delete = 127, return = 13
		//write("TextField key(", key, ") = [", key asCharacter, "]\n")
		keyAction := keyActions at(key asCharacter)
		//write("keyAction = ", keyAction type, "\n")
		if(keyAction, if(keyAction send == nil, return))
		
		if(key == GLUT_KEY_DEL,   deleteNextChar; return)
		if(key == GLUT_KEY_DELETE, deletePreviousChar; return)
		if(key == 13,  doAction; return)
		if(endCursorPosition, deleteSelection)
		insertString(key asCharacter)
		cursorPosition = cursorPosition + 1
    )
    
    special := method(key, x, y,
		// left = 100, right = 102
		//write("TextField special(", key, ")\n")
		if(key == GLUT_KEY_RIGHT, incrementCursorPosition; return)
		if(key == GLUT_KEY_LEFT, decrementCursorPosition; return)
    )
    
    insertString := method(s, 
		string atInsertSeq(cursorPosition, s)
    )

    firstCursorPosition := method(
		p := cursorPosition 
		if(endCursorPosition, p = p min(endCursorPosition), p)
    )
    
    lastCursorPosition := method(
		p := cursorPosition 
		if(endCursorPosition, p = p max(endCursorPosition), p)
    )    
    
    deleteSelection := method(
		c1 := firstCursorPosition
		c2 := lastCursorPosition
		string removeSlice(c1, c2)
		cursorPosition = c1
		endCursorPosition = nil
    )
    
    constrainCursor := method(cursorPosition = cursorPosition max(0) min(string size))
    
    deleteNextChar := method(
		if(endCursorPosition) then(deleteSelection) else(
			if(cursorPosition == string size, return)	
			string removeSlice(cursorPosition, cursorPosition + 1)
		)
        constrainCursor
    )
        
    deletePreviousChar := method(
		if(endCursorPosition) then(deleteSelection) else(
			if(cursorPosition == 0, return)
			cursorPosition = cursorPosition - 1
			cursorPosition = cursorPosition max(0) 
			string removeSlice(cursorPosition, cursorPosition + 1)
		)
        constrainCursor
    )
    
    incrementCursorPosition := method(
		if(endCursorPosition) then(
			cursorPosition = lastCursorPosition
			endCursorPosition = nil
		) else(
			cursorPosition = cursorPosition + 1
		)
        constrainCursor
    )
    
    decrementCursorPosition := method(
		if(endCursorPosition) then(
			cursorPosition = firstCursorPosition
			endCursorPosition = nil
		) else(
			cursorPosition = cursorPosition - 1
		)
        constrainCursor
    )
    
    tripleLeftMouseDown := method(
    	selectAll
		glutPostRedisplay
    )

    tripleLeftMouseUp := method(
    	selectAll
		glutPostRedisplay
    )
    
    selectAll := method(
    	cursorPosition = 0
    	endCursorPosition = string size
    )  
)




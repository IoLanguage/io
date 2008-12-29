#!/usr/bin/env io

Flux

app := Application clone do(	
	setTitle("Flux")
	mainWindow resizeTo(800, 800)

	keyboard := method(key, x, y,
	    resend
	    if(key == GLUT_KEY_ESC, mainWindow toggleFullScreen)
	)

	appDidStart := method(
	    resend
	    //mainWindow toggleFullScreen
	)
)

CloseButton := View clone do(
    target ::= nil
    size set(20, 20)
    
    draw := method(
        glColor4d(1, 1, 1, 1)
        size drawLineLoopi
        glBegin(GL_LINES)
        glVertex2d(0, 0)
        glVertex2d(size x, size y)
        glVertex2d(0, size y)
        glVertex2d(size x, 0)
        glEnd
    )
    
    leftMouseDown := method(
        nil
    )
)

Lobby type := "Lobby"

ObjectView := View clone do(
    title ::= "Lobby"
    next ::= nil
    previous ::= nil
    
    setTarget := method(t, 
        writeln("OV setTarget")
        self target := getSlot("t")
        self closeButton := CloseButton clone
        addSubview(closeButton)
        update
        self
    )
    
    update := method(
        self subviews empty
        if(self getSlot("target") slotNames size == 0, self getSlot("target") protos := method(resend))
        self getSlot("target") slotNames sort foreach(n, 
            sv := SlotView clone setTitle(n) 
            sv target := self getSlot("target")
            if(subviews last, sv placeAbove(subviews last, 0))
            addSubview(sv)
       )
        sizeToSubviews
        size += vector(0, 25)
        closeButton position set(0, 0)
   )

    drawOutline := method(
        if(isFirstResponder, glColor4d(1, 1, 1, .4), glColor4d(1, 1, 1, .2))
        size drawLineLoopi
        if(isFirstResponder not,
            glColor4d(0, 0, 0, .5)
            size drawQuad
        )
    )
    
    draw := method(
        glColor4d(.15,.15,.15,.9)
        size drawQuad
        glColor4d(1, 1, 1, 1)
        glTranslatei(10, size height - 25 + 5, 0)
		fonts system bold drawString(title)
    )
    
    leftMouseDown := method(
        makeFirstResponder
        orderFront
    )
    
    leftMouseMotion := method(
        if(previous and previous next == self, previous setNext(nil))
        moveWithMouseMotion
    )
    
    keyboard := method(k,
        if(k == 24, doRemoveAnimation)
    )
    
    doRemoveAnimation := method(
        self velocity := vector(0, 0); self timer := self getSlot("removeAnimation"); timer
    )
    
    remove := method(
        sv := superview
        removeFromSuperview
        if(previous, previous makeFirstResponder, sv subviews first ?makeFirstResponder)
    )
    
    removeAnimation := method(
        position += velocity
        velocity += vector(1, .5)
        glutPostRedisplay
        if(position x > topWindow size x, remove, topWindow addTimerTargetWithDelay(self, .01))
    )
)

SlotView := View clone do(
    title ::= "slot"
    target ::= nil
    position set(0,0)
    size set(160, 23)
    
    draw := method(
        glColor4d(1, 1, 1, 1)
        font := fonts system normal
		glTranslatei(20, (size height - font pixelHeight *.7) / 2, 0)
		font drawString(title)
    )
    
    theTarget := method(self getSlot("target"))
    
    objectView := method(superview)
    
    leftMouseDown := method(
        v := theTarget getSlot(title)
        //writeln(theTarget type, " ", title, " ", getSlot("v") type)
        ov := ObjectView clone setTarget(getSlot("v")) setTitle(getSlot("v") type .. "_" .. getSlot("v") uniqueId) 
        writeln("adding ov ", ov)
        app mainWindow subviews first addSubview(ov)
        ov placeRightOf(objectView)
        ov alignTopWith(objectView)
        ov makeFirstResponder orderFront
        objectView next ?doRemoveAnimation
        objectView setNext(ov)
        ov setPrevious(objectView)
    )
)

//contentView := View clone setSize(app mainWindow size) resizeWithSuperview
//contentView leftMouseDown := method(makeFirstResponder)

//app mainWindow addSubview(contentView)
//contentView makeFirstResponder

app mainWindow contentView addSubview(ObjectView clone setTarget(Lobby))

app run

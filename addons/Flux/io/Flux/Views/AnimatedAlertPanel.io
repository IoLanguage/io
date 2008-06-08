
// aView animate(setBackgroundColor(Color Red)) setDuration(.1)
// aView animate(backgroundColor, Color Red)) setDuration(.1)

View animate := method(
    call argAt(0)
)


AlertPanel := View clone do(
    alertMessage ::= "ALERT"
    window ::= nil
    setResizeWidth(010)
    setResizeHeight(011)
    
    font := fonts system bold
    fontColor ::= Color clone set(1, 1, 1,.6)
    setBackgroundColor(Color clone set(1,0,0,.8))
    
    sizeToMessage := method(
        size setWidth(font widthOfString(alertMessage)+50)
        size setHeight(font pixelSize * 2.5)
        self
    )
    
    draw := method(
        drawBackground
        /*
        backgroundColor glColor
        slices := 20
        offset := 5
        gluRoundedBox(quadric, width, height, offset, slices)
        Color clone set(1,0,0,.5)
        gluRoundedBoxOutline(quadric, width, height, offset + 1, slices)
        */
           
        glTranslatei(
                (width - font widthOfString(alertMessage)) / 2, 
                (height-font pixelSize)/2, 
                0)
        fontColor glColor
        font drawString(alertMessage)

    )
    
    openInWindow := method(w,
        setWindow(w)
        window contentView addSubview(self)
        self orderFront
    )
    
    close := method(
        removeFromSuperview
    )
    
    animateTo := method(endPoint,
        AttributeAnimator clone setAttributeName("position") setEndValue(endPoint) setWindow(window) setTarget(self) run
    )
    
    leftMouseDown := method(
        animateOut
    )
    
    animateOut := method(
        endPoint := position clone setY(window height)
        animator setAttributeName("position") setEndValue(endPoint) setEndAction("close") run 
    )
)

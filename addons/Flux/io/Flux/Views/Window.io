

WindowTitleBar := Button clone do(
    setTexturesPath("Window/TitleBar")
    resizeWidth = 101
    resizeHeight = 011
)

WindowResizer := Button clone do(
    protoName := "WindowResizer"
    setTexturesPath("Window/GrowBox")
    size set(16,16)
    resizeWidth = 011
    resizeHeight = 110
)

WindowCloseButton := Button clone do(
    resizeHeight := 011
    setTexturesPath("Window/TitleBar/CloseButton")
    size set(16, 16)
    clippingOff
)

//-------------------------------------------------

Window := View clone do(
    isSelected := 0
    name := "Window"
    setTitle := method(t, titleBar setTitle(t))
    size setWidth(300) setHeight(300)
    titleHeight  := 22
    resizeWidth  := 101
    resizeHeight := 101
    minSize   := Point clone set(100,100,0)
    setSize   := method(aSize, size setWidth(aSize width) setHeight(aSize height))
    textColor := Color clone set(1,1,1,1)

    backgroundTextures := TextureGroup clone loadGroupNamed("Window")
    titleBarTextures   := TextureGroup clone loadGroupNamed("Window/TitleBar")
    shadowTextures   := TextureGroup clone loadGroupNamed("Window/Shadow")
    
    init := method(
		resend
		self resizer := WindowResizer clone
		resizer position setX(size width - resizer size width - 1) setY(1)
		resizer motionAction := "grow"
		resizer setActionTarget(self)
		addSubview(resizer)
		
		self titleBar := WindowTitleBar clone
		titleBar position set(0, height - 22)
		titleBar size set(width - 1, 32)
		titleBar motionAction := "move"
		titleBar setActionTarget(self)
		addSubview(titleBar)
		
		self closeButton := WindowCloseButton clone
		closeButton position set(6, 2)
		closeButton setActionTarget(self)
		closeButton setAction("close")
		titleBar addSubview(closeButton)
	)
		
	display := method(
		glPushMatrix
		position glTranslate
		drawShadow
		glPopMatrix
		resend
    )

    draw := method(
		backgroundColor glColor
		size drawQuad
		//backgroundTextures draw(width, height)
		//drawTitleBar
    )

    buttonClick := method(
		"Window buttonClick\n" print
		glutPostRedisplay
    )

    drawTitleBar := method(
		glPushMatrix
		glTranslatei(0, height - 24, 0)
		titleBarTextures draw(width, 32)
		glPopMatrix
    )

    drawShadow := method(
		glPushMatrix
		glTranslatei(-7, -10, 0)
		shadowTextures draw(width + 14, height + 13)
		glPopMatrix
    )

    leftMouseDown := method(
		Screen orderToFrontSubview(self)
		glutPostRedisplay
    )

    grow := method(
		//write("Mouse difference x := ", Mouse difference x, "\n")
		p := Mouse difference
		p setY(- p y)
		currentHeight := size height
		resizeBy(p x, p y)
		position setY(position y - size height - currentHeight)
		glutPostRedisplay
    )

    move := method(
		p := Mouse difference
		position += p
		glutPostRedisplay
    )
    
    open := method(
		Screen addSubview(self)
		Screen orderToFrontSubview(self)
    )
    
    close := method(
		Screen removeSubview(self)
    )
)

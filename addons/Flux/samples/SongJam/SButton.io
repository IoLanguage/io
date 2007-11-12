
Bar := Button clone do(
    fillImage  := ImageManager item("Window/TitleBar/Middle")
    draw := method(fillImage drawTexture(width, fillImage height))
)

MiniBar := Button clone do(
    fillImage  := ImageManager item("Scroller/Horizontal/Cover")
    draw := method(fillImage drawTexture(width, fillImage height))
)

FlatButton := Button clone do(

    //clippingOff
    draw := method(
      w := size width
      h := size height

      a := .3
      c := .4
      sh := h*.7 

      // draw bottom middle shading
      glBegin(GL_QUADS)
	glColor4d(c, c, c, 1)
	glVertex3d(0, 0, 0) 
	glColor4d(a, a, a, 1)
	glVertex3d(0, sh, 0)
	glVertex3d(w, sh, 0)
	glColor4d(c, c, c, 1)
	glVertex3d(w, 0, 0)
      glEnd

      // draw top middle white
      glColor4d(a, a, a, 1)
      glRectd(0, sh, w, h)
    )
    
    drawOutline := method(
	outlineColor glColor
	glLineWidth(1)
	size drawLineLoopi
    )
)

// ------------------------------------------

SButton := Button clone do(
    resizeWidth  = 110
    resizeHeight = 011
    size setWidth(80)
)

MusicButton := Button clone do(
    resizeWidth  = 110
    resizeHeight = 011
    textureWidth  := 64
    textureHeight := 64
    size set(42, 42)
)

// ---------------------------------------------------------------------

PlayButton := MusicButton clone do(
    isOn := nil

    setTexturesPath("Buttons/Music/Pause")
    pauseTextures := textures 
    
    setTexturesPath("Buttons/Music/Play")
    playTextures := textures

    turnOff := method(
	isOn = nil
	textures = playTextures
    )
    
    turnOn := method(
	isOn = 1
	textures = pauseTextures
    )    
    
    leftMouseUp := method(
        if (isDisabled, return)
	resend
	//if (isOn, turnOff, turnOn)
    )
)


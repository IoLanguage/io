
Button := View clone do(
    position set(100, 100)
    size set(100,32)

    textures := TextureSet clone

    setTexturesPath := method(path,
        textures = TextureSet clone openSet(path)
    )
    
    setTexturesPath("Buttons/Push")

    isSelected := false
    select     := method(isSelected = true)
    unselect   := method(isSelected = false)    
    
    isDisabled := false
    disable    := method(isDisabled = true)
    enable     := method(isDisabled = false)
    
    newSlot("title", "")
    widthOfTitle := method(font widthOfString(title))
    sizeToTitle := method(setWidth(widthOfTitle + 40); self)
    
    motionAction := nil
    
    newSlot("textColor", Color clone set(0,0,0,1))
    newSlot("textColorSelected", Color clone set(0,0,0,1))
    newSlot("textColorDisabled", Color clone set(.5,.5,.5,1))

    newSlot("image")
    newSlot("altImage")

    sizeToImage := method(
      if(image, size setWidth(image width) setHeight(image height+1))
    )

    textureWidth := method(width)
    textureHeight := method(height)
	roundingSize := 0
    
    draw := method(
		if(image, 
			if(isSelected not) then(
				image draw
			) else(
				if(altImage, altImage, image) draw
			)
			return
		)
		
		w := size width
		h := size height
        glColor4d(1,1,1,1)
		
		if(isTextured,
            if(isDisabled) then(
                textures disabled draw(textureWidth, textureHeight)
            ) else(
                if(isSelected,
                    textures active draw(textureWidth, textureHeight)
                ) else(
                    textures normal draw(textureWidth, textureHeight)
                )
            ) 
		) else(
            if(isDisabled) then(
               disabledColor glColor
            ) else(
                if(isSelected) then(
                    selectedColor glColor
                ) else(
                    boxColor glColor
                )
            )
            drawRoundedBox 
		)
				
		// draw label
		glPushMatrix
		textColor glColor
		//if(isSelected, textColorSelected glColor)
		if(isDisabled, textColorDisabled glColor)
		b := ((w - font widthOfString(title)) * .5) round
		glTranslatei(b, (3 + size y - font pixelHeight) / 2, 0)
		font drawString(title)
		glPopMatrix
    )

    drawBackground := nil
    drawOutline := nil

    newSlot("previousFirstResponder")
    
    pushFirstResponder := method(
        setPreviousFirstResponder(topWindow firstResponder)
        makeFirstResponder
    )
    
    popFirstResponder := method(
        previousFirstResponder ?makeFirstResponder
        setPreviousFirstResponder(nil)
    )
    
    leftMouseDown := method(
		if(isDisabled, return)
		pushFirstResponder
		select
		setNeedsRedraw(true)
    )

    leftMouseUp := method(
		if(isDisabled, return)
		if(isSelected, doAction)
		unselect
		setNeedsRedraw(true)
		popFirstResponder
    )

    leftMouseMotion := method(
		if(isDisabled, return)
		isSelected = if(screenHit, true, false)
		//write("isSelected := ", isSelected, "\n")
		//if(isSelected, 
		if(actionTarget and motionAction, actionTarget perform(motionAction, self)) 
		//)
		glutPostRedisplay
    )
)

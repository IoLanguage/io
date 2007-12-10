TextureGroup := GLObject clone do(    

    /*
    draw := method(width, height,
        writeln("TextureGroup draw")
        if (width < 1 or height < 1, return)
    
        if (?bottomLeft, drawAll(width, height))
		elseif (?left,   drawLeftRight(width, height))
		elseif (?bottom, drawBottomTop(width, height))
        elseif (?middle, middle drawTexture(width, height))
    )
    */
    draw := nil
    
    setDrawMethod := method(
		self draw = nil
		if(?bottomLeft) then(self draw = self getSlot("drawAll")
                ) elseif(?left) then(self draw = self getSlot("drawLeftRight")
                ) elseif(?bottom) then(self draw = self getSlot("drawBottomTop")
                ) elseif(?middle) then(self draw = self getSlot("drawMiddle"))
    )
    
    drawMiddle := method(width, height,
    	glColor4d(1,1,1,1)
		middle drawTexture(width, height)
    )

    drawLeftRight := method(width, height,
    	glColor4d(1,1,1,1)
		left drawTexture
		
		glPushMatrix
		glTranslatei(left width , 0, 0)
		middle drawTexture(width - left width - right width, height)
		glPopMatrix
		
		glPushMatrix
		glTranslatei(width - right width, 0, 0)
		right drawTexture
		glPopMatrix
    )
    
    drawBottomTop := method(width, height,
    	glColor4d(1,1,1,1)
		glPushMatrix
		glTranslatei(0, -1, 0)
		bottom drawTexture
		glPopMatrix
		
		glPushMatrix
		glTranslatei(0, bottom height - 1, 0)
		middle drawTexture(width, height - bottom height - top height + 1)
		glPopMatrix
		
		glPushMatrix
		glTranslatei(0, height - top height, 0)
		top drawTexture
		glPopMatrix
    )
    
    drawAll := method(width, height,	
    	glColor4d(1,1,1,1)
		// middle
		if (middle, middle drawTexture(width, height))
	
		// left
		glPushMatrix
		glTranslatei(0, bottomLeft height, 0)
		left drawTexture(left width, height - top height - bottomLeft height)	
		glPopMatrix
	
		// bottom
		glPushMatrix
		bottomLeft drawTexture
		glTranslatei(bottomLeft width, 0, 0)
		bottom drawTexture(width - bottomLeft width - bottomRight width, bottom height)
		glPopMatrix
	
		// bottomRight, right, topRight
		glPushMatrix 
		glTranslatei(width - left width, 0, 0)
		bottomRight drawTexture
		glPushMatrix
		glTranslatei(0, bottomLeft height, 0)
		right drawTexture(right width, height - top height - bottomLeft height)
		glPopMatrix
		glTranslatei(0, height - topRight height, 0)
		topRight drawTexture
		glPopMatrix
		
		// topLeft, top
		glPushMatrix 
		glTranslatei(0, height - topLeft height, 0)
		topLeft drawTexture
		glTranslatei(topLeft width, 0, 0)
		top drawTexture(width - topLeft width - topRight width, top height)
		glPopMatrix
    )

    loadGroupNamedOrNil := method(name, 
		loadGroupNamed(name, 1)
    )
    
    loadGroupNamed := method(name, noException,
		self path := name
		//write("TextureGroup loadGroupNamed(", name, ")\n")
		self bottomLeft  := ImageManager itemOrNil(Path with(name, "BottomLeft"))
		self left        := ImageManager itemOrNil(Path with(name, "Left"))
		self topLeft     := ImageManager itemOrNil(Path with(name, "TopLeft"))
		self top         := ImageManager itemOrNil(Path with(name, "Top"))
		self topRight    := ImageManager itemOrNil(Path with(name, "TopRight"))
		self right       := ImageManager itemOrNil(Path with(name, "Right"))
		self bottomRight := ImageManager itemOrNil(Path with(name, "BottomRight"))
		self bottom      := ImageManager itemOrNil(Path with(name, "Bottom"))
		self middle      := ImageManager itemOrNil(Path with(name, "Middle"))
		setDrawMethod
		if(self getSlot("draw") == nil,
			if(noException, 
				write("no draw method for ", name, "\n")
				return nil
			) 
			Exception raise("TextureGroup unable to find textures in set " .. name .. "\n")
		)
		self     
    )
)

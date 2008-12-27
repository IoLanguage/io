#!/usr/bin/env io

// [reg:~/IoProject/Io/binaries] steve% ./ioDesktop ../bindings/Font/_tests/slides.io 

//doFile(Path with(System launchPath, "GLApp.io"))

Slide := Object clone do(
    bigFont    := Font clone open("../../Flux/resources/fonts/Free/Sans/Bold.ttf")   setPixelSize(200)
    mediumFont := Font clone open("../../Flux/resources/fonts/Free/Sans/Bold.ttf")   setPixelSize(48)
    smallFont  := Font clone open("../../Flux/resources/fonts/Free/Sans/Normal.ttf") setPixelSize(32)
    
    titleFont := mediumFont
    textFont  := smallFont
    
    appendProto(OpenGL)
    
    title ::= ""
    bulletPoints ::= ""
    width  := method(app width)
    height := method(app height)
    titleRatio := .95
    spacerRatio := 4
    bulletsCentered := nil
    pointsCentered := nil
    
    drawBackground := method(
    	glBegin(GL_QUADS)
		glColor4d(.5, .5, .6, 1)
		glVertex2d(0, 0)
		glVertex2d(width, 0)
		glColor4d(0, 0, 0, 1)
		glVertex2d(width, height)
		glVertex2d(0, height)
		glEnd
    )

    draw := method(
		drawBackground
		glColor4d(1, 1, 1, 1)
		drawTitle
		glTranslatei(0, - textFont pixelSize * spacerRatio, 0)
		drawBulletPoints
    )
    
    drawTitle := method(
		glTranslatei(0, (height * titleRatio) - (titleFont pixelSize), 0)
		glPushMatrix
		titleFont drawCenteredString(title, width) 
		glPopMatrix
    )
    
    drawBulletPoints := method(centered,
		bulletPoints foreach(line,
			line := line asMutable replaceSeq("\t", "    ")
			glPushMatrix
			drawBulletPoint(line)
			glPopMatrix
			glTranslatei(0, - (textFont pixelSize * 1.6), 0)
		)    
    )
    
    drawBulletPoint := method(line,
		if(pointsCentered) then(
			textFont drawCenteredString(line, width)	
		) else(
			glTranslatei(width * .1, 0, 0)
			textFont drawString(line)
		)
    )

    makeFrontPage := method(
		titleFont = bigFont
		bulletsCentered = 1
		titleRatio = .7
		spacerRatio = 2
		pointsCentered = 1
    )
)

Slideshow := GLApp clone do(
    slideNumber := 0
    
    open := method(path,
		file := File with(path)
		if (file isRegularFile not,
			"Supplied file is not a regular file" println
			return
		)
		parse(file contents asString)
		runFullScreen
    )
    
    parse := method(data,
		self slides := List clone	
		data splitNoEmpties("\n\n") foreach(slideText,
			lines := slideText splitNoEmpties("\n")
			slide := Slide clone
			slide setTitle(lines removeAt(0))
			slide setBulletPoints(lines) 
			slide app := self 
			slides append(slide)
			if (slides size == 1, slide makeFrontPage)
		)
    )
    
    draw := method(slides at(slideNumber) draw)
    escapeKey      := method(System exit)
    specialUpKey   := method(slideNumber = ((slideNumber - 1) max(0)))
    specialDownKey := method(slideNumber = ((slideNumber + 1) min(slides size - 1)))
)

Slideshow open(System args at(1))

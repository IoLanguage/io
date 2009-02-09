#!/usr/bin/env io

Flux 

Slide := Object clone do(
    bigFont    := FontManager item("Free/Sans/Bold") setPixelSize(200)
    mediumFont := bigFont clone open setPixelSize(48)
    smallFont  := bigFont clone open setPixelSize(32)
    
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
		glTranslatei(0, (height * titleRatio) - titleFont pixelSize, 0)
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
			glTranslatei(0, - textFont pixelSize * 1.6, 0)
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
    	if(file exists not, Exception raise(path .. " not found"))
    	//writeln(file contents)
		parse(file contents)
		run
    )
    
    parse := method(data,
		self slides := List clone	
		data split("\n\n") foreach(slideText,
			lines := slideText split("\n")
			slide := Slide clone
			slide setTitle(lines removeAt(0))
			slide setBulletPoints(lines) 
			slide app := self 
			slides append(slide)
			if (slides size == 1, slide makeFrontPage)
		)
    )
    
    draw := method(slides at(slideNumber) draw)
    escapeKey      := method(toggleFullScreen)
    specialUpKey   := method(slideNumber = (slideNumber - 1) max(0))
    specialDownKey := method(slideNumber = (slideNumber + 1) min(slides size - 1))
)

//Slideshow open(if(System args size > 0, System args at(1), "slides.txt"))
path := Path with(System launchPath, "slides.txt")
Slideshow open(path)


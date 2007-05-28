
Panel := Window clone do(
    backgroundColor := Color clone set(0,0,0,1)
    
    draw := method(
		backgroundColor glColor
		size drawQuad    
		backgroundTextures draw(width-1, height-1)
    )
    
    
    open := method(
		x := (Screen width - width) / 2
		y := (Screen height - height) / 2
		position set(x, y)
		resend
    )
    
    shake := method(
		x0 := position x
		for (i, 1, 5,
			position setX(x0-5)
			Screen display
			wait(.01)
			position setX(x0+5)
			Screen display
			wait(.01)
		)
		position setX(x0)
    )
)

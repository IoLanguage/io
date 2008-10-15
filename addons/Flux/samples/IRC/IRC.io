
Screen setSize(800, 400)

IRCApp := Object clone 
IRCApp do(
    IRCView := View clone do(
      size set(Screen width, Screen height)
      resizeWithSuperview 
      textures := TextureGroup clone loadGroupNamed("Screen")
      draw := method(
	textures draw(width, height)
      )
    )    

    IRCView setNextResponder(IRCApp)

    
)

Screen addSubview(IRCApp IRCView)
Screen setTitle("Terminal")
Screen open

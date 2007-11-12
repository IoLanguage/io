
VScroller := HScroller clone do(
    resizeWidth  := 011
    resizeHeight := 101
    axis         = "y"
    setAxis      = "setY"
    otherAxis    = "x"
    setOtherAxis = "setX"
    defaultValue = 1
	
    knob = VKnob clone

    setXProportion := method(nil)
    setYProportion := method(p, setProportion(p))
    
    textures := TextureGroup clone loadGroupNamed("Scroller/Vertical")

    size setWidth(textures top width)
)
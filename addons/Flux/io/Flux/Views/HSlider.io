
HSlider := HScroller clone do(
    textures := TextureGroup clone loadGroupNamed("Slider/Horizontal")
    size setHeight(textures left height)
    
    knob textures := TextureGroup clone loadGroupNamed("Slider/Horizontal/Knob")
    knob maxSpace := 16
    knob setWidth(16)
    

    draw := method(
	textures draw(width, height)
        knob setWidth(15)
    )
)




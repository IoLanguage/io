
VKnob := HKnob clone do(
    size set(20,20)
    resizeWidth  := 010
    resizeHeight := 101
    makeVertical 
    //minSize := vector(0,40)
    textures := TextureGroup clone loadGroupNamed("Scroller/Vertical/Knob")
)


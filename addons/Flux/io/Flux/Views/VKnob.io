
VKnob := HKnob clone 

VKnob do(
    size set(20,20)
    resizeWidth  := 010
    resizeHeight := 101
    makeVertical 

    textures := TextureGroup clone loadGroupNamed("Scroller/Vertical/Knob")
)


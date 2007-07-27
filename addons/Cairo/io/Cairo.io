
Cairo := Object clone do(
	Context := CairoContext
	
	Surface := Object clone do(
		Image := CairoSurfaceImage
	)
	
	Format := Object clone do(
	    ARGB32 := 0
	    RGB24  := 1
	    A8     := 2
	    A1     := 3
	    /* The value of 4 is reserved by a deprecated enum value.
	     * The next format added must have an explicit value of 5.
	    CAIRO_FORMAT_RGB16_565 = 4,
	    */	
	)
	
	Font := Object clone do(
		Slant := Object clone do(
			Normal  := 0
			Italic  := 1
			Oblique := 2
		)
		
		Weight := Object clone do(
			Normal := 0
			Bold   := 1
		)
	)
	
	TextExtents := List clone do(
		xBearing := method(at(0))
		yBearing := method(at(1))
		width    := method(at(2))
		height   := method(at(3))
		xAdvance := method(at(4))
		xAdvance := method(at(5))
	)
	
	Pattern := CairoPattern do(
		Solid := CairoPatternSolid
		Gradient := CairoPatternGradient do(
			Linear := CairoPatternGradientLinear
			Radial := CairoPatternGradientRadial
		)
	)
)

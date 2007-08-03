
Cairo := Object clone do(
	Context := CairoContext

	ImageSurface := CairoImageSurface
	
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
	
	TextExtents := CairoTextExtents
	FontExtens := CairoFontExtents

	Pattern := CairoPattern
	SolidPattern := CairoSolidPattern
	Gradient := CairoGradient
	LinearGradient := CairoLinearGradient
	RadialGradient := CairoRadialGradient
)

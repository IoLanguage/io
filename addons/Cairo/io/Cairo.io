Cairo do(
	Context := CairoContext
	
	Path := CairoPath
	PathElement := CairoPathElement

	Surface := CairoSurface
	ImageSurface := CairoImageSurface
	if (HAS_PS_SURFACE, PSSurface := CairoPSSurface)
	if (HAS_PDF_SURFACE, PDFSurface := CairoPDFSurface)
	if (HAS_SVG_SURFACE, SVGSurface := CairoSVGSurface)
	
	FontFace := CairoFontFace
	ScaledFont := CairoScaledFont
	FontOptions := CairoFontOptions
	FontExtents := CairoFontExtents
	TextExtents := CairoTextExtents
	Glyph := CairoGlyph

	Extents := CairoExtents
	Rectangle := CairoRectangle

	Pattern := CairoPattern
	SolidPattern := CairoSolidPattern
	Gradient := CairoGradient
	LinearGradient := CairoLinearGradient
	RadialGradient := CairoRadialGradient
	SurfacePattern := CairoSurfacePattern
	
	Matrix := CairoMatrix
)

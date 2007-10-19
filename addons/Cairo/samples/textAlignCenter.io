Sample context do(
	text := "cairo"
	
	selectFontFace("Sans", FONT_SLANT_NORMAL, FONT_WEIGHT_NORMAL)
	setFontSize(0.2)

	extents := textExtents(text)

	x := 0.5 - (extents width / 2 + extents xBearing)
	y := 0.5 - (extents height / 2 + extents yBearing)

	moveTo(x, y)
	showText(text)

	# draw helping lines
	setSourceRGBA(1, 0.2, 0.2, 0.6)
	arc(x, y, 0.05, 0, 2 * pi)
	fill
	moveTo(0.5, 0)
	relLineTo(0, 1)
	moveTo(0, 0.5)
	relLineTo(1, 0)
	stroke
)

Sample context do(
	text := "cairo"

	selectFontFace("Sans", FONT_SLANT_NORMAL, FONT_WEIGHT_NORMAL)
	setFontSize(0.4)
	extents := textExtents(text)

	x := 0.1
	y := 0.6

	moveTo(x, y)
	showText(text)

	# draw helping lines
	setSourceRGBA(1, 0.2, 0.2, 0.6)
	arc(x, y, 0.05, 0, 2 * pi)
	fill
	moveTo(x, y)
	relLineTo(0, -extents height)
	relLineTo(extents width, 0)
	relLineTo(extents xBearing, -extents yBearing)
	stroke
)
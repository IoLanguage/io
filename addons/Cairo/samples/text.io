Sample context do(
	selectFontFace("Sans", FONT_SLANT_NORMAL, FONT_WEIGHT_BOLD)
	setFontSize(0.35)

	moveTo(0.04, 0.53)
	showText("Hello")

	moveTo(0.27, 0.65)
	textPath("void")
	setSourceRGB(0.5, 0.5, 1)
	fillPreserve
	setSourceRGB(0, 0, 0)
	setLineWidth(0.01)
	stroke

	# draw helping lines
	setSourceRGBA(1, 0.2, 0.2, 0.6)
	arc(0.04, 0.53, 0.02, 0, 2 * pi)
	closePath
	arc(0.27, 0.65, 0.02, 0, 2 * pi)
	fill
)

	
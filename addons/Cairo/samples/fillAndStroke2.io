Sample context do(
	moveTo(0.5, 0.1)
	lineTo(0.9, 0.9)
	relLineTo(-0.4, 0)
	curveTo(0.2, 0.9, 0.2, 0.5, 0.5, 0.5)
	closePath

	moveTo(0.25, 0.1)
	relLineTo(0.2, 0.2)
	relLineTo(-0.2, 0.2)
	relLineTo(-0.2, -0.2)
	closePath

	setSourceRGB(0, 0, 1)
	fillPreserve
	setSourceRGB(0, 0, 0)
	stroke
)

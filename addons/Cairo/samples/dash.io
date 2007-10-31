Sample context do(
	dashes := list(
		0.20, # ink
		0.05, # skip
		0.05, # ink
		0.05	# skip
	)

	setDash(dashes, -0.2)
	moveTo(0.5, 0.1)
	lineTo(0.9, 0.9)
	relLineTo(-0.4, 0)
	curveTo(0.2, 0.9, 0.2, 0.5, 0.5, 0.5)
	stroke
)

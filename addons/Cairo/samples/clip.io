Sample context do(
	arc(0.5, 0.5, 0.3, 0, 2 * pi)
	clip

	rectangle(0, 0, 1, 1)
	fill

	setSourceRGB(0, 1, 0)
	moveTo(0, 0)
	lineTo(1, 1)
	moveTo(1, 0)
	lineTo(0, 1)
	stroke
)

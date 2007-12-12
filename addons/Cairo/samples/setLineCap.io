Sample context do(
	setLineWidth(0.12)
	setLineCap(LINE_CAP_BUTT)
	moveTo(0.25, 0.2) lineTo(0.25, 0.8)
	stroke

	setLineCap(LINE_CAP_ROUND)
	moveTo(0.5, 0.2) lineTo(0.5, 0.8)
	stroke

	setLineCap(LINE_CAP_SQUARE)
	moveTo(0.75, 0.2) lineTo(0.75, 0.8)
	stroke

	# draw helping lines
	setSourceRGB(1, 0.2, 0.2)
	setLineWidth(0.01)
	moveTo(0.25, 0.2) lineTo(0.25, 0.8)
	moveTo(0.5, 0.2) lineTo(0.5, 0.8)
	moveTo(0.75, 0.2) lineTo(0.75, 0.8)
	stroke
)

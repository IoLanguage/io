Sample context do(
	setLineWidth(0.16)
	moveTo(0.3, 0.33)
	relLineTo(0.2, -0.2)
	relLineTo(0.2, 0.2)
	setLineJoin(LINE_JOIN_MITER)
	stroke

	moveTo(0.3, 0.63)
	relLineTo(0.2, -0.2)
	relLineTo(0.2, 0.2)
	setLineJoin(LINE_JOIN_BEVEL)
	stroke

	moveTo(0.3, 0.93)
	relLineTo(0.2, -0.2)
	relLineTo(0.2, 0.2)
	setLineJoin(LINE_JOIN_ROUND)
	stroke
)
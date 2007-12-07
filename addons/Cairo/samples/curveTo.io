Sample context do(
	x := 0.1
	y := 0.5

	x1 := 0.4
	y1 := 0.9

	x2 := 0.6
	y2 := 0.1

	x3 := 0.9
	y3 := 0.5

	moveTo(x, y)
	curveTo(x1, y1, x2, y2, x3, y3)
	stroke

	setSourceRGBA(1, 0.2, 0.2, 0.6)
	setLineWidth(0.03)
	moveTo(x, y) lineTo(x1, y1)
	moveTo(x2, y2) lineTo(x3, y3)
	stroke
)

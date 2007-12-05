Sample context do(
	x0 := 0.1
	y0 := 0.1
	width := 0.8
	height := 0.8
	radius := 0.4

	x1 := x0 + width
	y1 := y0 + height

	if ((width / 2) < radius,
		if ((height / 2) < radius,
			moveTo(x0, (y0 + y1) / 2)
			curveTo(x0 ,y0, x0, y0, (x0 + x1) / 2, y0)
			curveTo(x1, y0, x1, y0, x1, (y0 + y1) / 2)
			curveTo(x1, y1, x1, y1, (x1 + x0) / 2, y1)
			curveTo(x0, y1, x0, y1, x0, (y0 + y1) / 2)
		) else (
			moveTo(x0, y0 + radius)
			curveTo(x0 ,y0, x0, y0, (x0 + x1) / 2, y0)
			curveTo(x1, y0, x1, y0, x1, y0 + radius)
			lineTo(x1 , y1 - radius)
			curveTo(x1, y1, x1, y1, (x1 + x0) / 2, y1)
			curveTo(x0, y1, x0, y1, x0, y1- radius)
		)
	) else (
		if ((height / 2) < radius,
			moveTo(x0, (y0 + y1) / 2)
			curveTo(x0 , y0, x0 , y0, x0 + radius, y0)
			lineTo(x1 - radius, y0)
			curveTo(x1, y0, x1, y0, x1, (y0 + y1) / 2)
			curveTo(x1, y1, x1, y1, x1 - radius, y1)
			lineTo(x0 + radius, y1)
			curveTo(x0, y1, x0, y1, x0, (y0 + y1) / 2)
		) else (
			moveTo(x0, y0 + radius)
			curveTo(x0 , y0, x0 , y0, x0 + radius, y0)
			lineTo(x1 - radius, y0)
			curveTo(x1, y0, x1, y0, x1, y0 + radius)
			lineTo(x1 , y1 - radius)
			curveTo(x1, y1, x1, y1, x1 - radius, y1)
			lineTo(x0 + radius, y1)
			curveTo(x0, y1, x0, y1, x0, y1- radius)
		)
	)
	closePath

	setSourceRGB(0.5, 0.5, 1)
	fillPreserve
	setSourceRGBA(0.5, 0, 0, 0.5)
	stroke
)

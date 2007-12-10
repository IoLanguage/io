Sample context do(
	xc := 0.5
	yc := 0.5
	radius := 0.4
	angle1 := 45 * (pi / 180) # angles are in radians
	angle2 := 180 * (pi / 180)

	arcNegative(xc, yc, radius, angle1, angle2)
	stroke

	# draw helping lines
	setSourceRGBA(1, 0.2, 0.2, 0.6)
	arc(xc, yc, 0.05, 0, 2 * pi)
	fill

	setLineWidth(0.03)
	arc(xc, yc, radius, angle1, angle1)
	lineTo(xc, yc)
	arc(xc, yc, radius, angle2, angle2)
	lineTo(xc, yc)
	stroke
)

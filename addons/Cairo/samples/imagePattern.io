Sample context do(
	image := ImageSurface createFromPNG(Path with("data", "romedalen.png"))
	w := image getWidth
	h := image getHeight

	pattern := SurfacePattern create(image)
	pattern setExtend(EXTEND_REPEAT)

	translate(0.5, 0.5)
	rotate(pi / 4)
	scale(1 / (2 sqrt), 1 / (2 sqrt))
	translate(-0.5, -0.5)

	matrix := Matrix clone scale(w * 5, h * 5)
	pattern setMatrix(matrix)

	setSource(pattern)
	rectangle(0, 0, 1, 1)
	fill
)

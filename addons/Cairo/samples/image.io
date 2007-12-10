Sample context do(
	image := ImageSurface createFromPNG(Path with("data", "romedalen.png"))
	w := image getWidth
	h := image getHeight

	translate(0.5, 0.5)
	rotate(45 * (pi / 180))
	scale(1.0 / w, 1.0 / h)
	translate(-0.5 * w, -0.5 * h)

	setSourceSurface(image, 0, 0)
	paint
)

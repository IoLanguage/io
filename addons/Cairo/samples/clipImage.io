Sample context do(
	image := ImageSurface createFromPNG(Path with("data", "romedalen.png"))
	w := image getWidth
	h := image getHeight

	arc(0.5, 0.5, 0.3, 0, 2 * pi)
	clip

	scale(1.0 / w, 1.0 / h)

	setSourceSurface(image, 0, 0)
	paint
)

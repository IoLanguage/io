Sample context do(
	pat := Cairo LinearGradient create(0, 0, 0, 1)
	pat addColorStopRGBA(1, 0, 0, 0, 1)
	pat addColorStopRGBA(0, 1, 1, 1, 1)
	setSource(pat)
	paint

	pat := Cairo RadialGradient create(
		0.45, 0.4, 0.1,
		0.4, 0.4, 0.5
	)
	pat addColorStopRGBA(0, 1, 1, 1, 1)
	pat addColorStopRGBA(1, 0, 0, 0, 1)
	setSource(pat)
	arc(0.5, 0.5, 0.3, 0, 2 * pi)
	fill
)

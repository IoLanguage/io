
List do(
	asCairoTextExtents := method(
		te := Cairo TextExtents clone
		foreach(x, te append(x))
		te
	)
)

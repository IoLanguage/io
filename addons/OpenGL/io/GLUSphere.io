GLUSphere := GLUQuadric clone do(
	radius ::= 0
	stacks ::= 0
	slices ::= 0

	draw := method(
		texture ifNonNil(texture bind)
		gluSphere(self, radius, slices, stacks)
		self
	)
)

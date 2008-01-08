GLUCylinder := GLUQuadric clone do(
	baseRadius ::= 0
	topRadius ::= 0
	height ::= 0
	stacks ::= 0
	slices ::= 0

	draw := method(
		texture ifNonNil(texture bind)
		gluCylinder(self, baseRadius, topRadius, height, slices, stacks)
		self
	)
)

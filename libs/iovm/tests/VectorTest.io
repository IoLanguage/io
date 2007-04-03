VectorTest := UnitTest clone do(
	testSum := method(
		assertEquals(1+2+3+4+5, vector(1,2,3,4,5) sum)
	)

	testProduct := method(
		assertEquals(1*2*3*4*5, vector(1,2,3,4,5) product)
	)

	testRootMeanSquare := method(
		assertEquals(list(1,2,3,4,5) map(**2) sum /(5) sqrt, vector(1,2,3,4,5) rootMeanSquare)
	)
)

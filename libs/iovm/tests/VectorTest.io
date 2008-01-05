VectorTest := UnitTest clone do(
	testSum := method(
		assertEquals(1+2+3+4+5, vector(1,2,3,4,5) sum)
	)

	testProduct := method(
		assertEquals(1*2*3*4*5, vector(1,2,3,4,5) product)
	)

	testRootMeanSquare := method(
		assertEquals((list(1,2,3,4,5) map(**2) sum / 5) sqrt, vector(1,2,3,4,5) rootMeanSquare)
	)

	testRemoveAt := method(
		assertRaisesException(vector(0, 1, 2) removeAt)
		assertRaisesException(vector(0, 1, 2) removeAt(nil))
		assertEquals(vector(1, 2), vector(0, 1, 2) removeAt(0))
		assertEquals(vector(0, 2), vector(0, 1, 2) removeAt(1))
		assertEquals(vector(0, 1), vector(0, 1, 2) removeAt(2))
	)

	testRemoveSlice := method(
		assertRaisesException(vector(0, 1, 2) removeSlice)
		assertRaisesException(vector(0, 1, 2) removeSlice(nil))
		assertRaisesException(vector(0, 1, 2) removeSlice(1, nil))
		assertRaisesException(vector(0, 1, 2) removeSlice(nil, 2))
		assertEquals(vector(0, 2), vector(0, 1, 2) removeSlice(1, 1))
		assertEquals(vector(0), vector(0, 1, 2) removeSlice(1, 2))
		assertEquals(vector(0, 1, 2), vector(0, 1, 2) removeSlice(2, 1))
		assertEquals(vector(2), vector(0, 1, 2) asMutable removeSlice(0, 1))
		assertEquals(vector(0, 5), vector(0, 1, 2, 3, 4, 5) removeSlice(1, 4))
		assertEquals(vector(), vector(0, 1, 2) removeSlice(0, 2))
		assertEquals(vector(0), vector(0, 1, 2) asMutable removeSlice(1, 8))
		assertEquals(vector(2), vector(0, 1, 2) removeSlice(-8, 1))
	)
)

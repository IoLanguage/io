VectorCompareTest := UnitTest clone do(
	#verbose := method(m, m println)

	testStrings := method(
		assertTrue("ab" == "ab")
		assertTrue("ba" == "ba")
		assertTrue("ab" != "ba")
		assertTrue("ab" < "ba")
		assertTrue("ab" <= "ba")
		assertTrue("ba" > "ab")
		assertTrue("ba" >= "ab")

		assertFalse("ab" == "ba")
		assertFalse("ba" == "ab")
		assertFalse("ba" < "ab")
		assertFalse("ba" <= "ab")
		assertFalse("ab" > "ba")
		assertFalse("ab" >= "ba")
	)

	testVectors := method(
		assertTrue( vector(1,1) == vector(1,1) )
		assertFalse( vector(1,0) == vector(0,1) )
		assertFalse( vector(0,1) == vector(1,0) )

		assertFalse( vector(1,1) != vector(1,1) )
		assertTrue( vector(1,0) != vector(0,1) )
		assertTrue( vector(0,1) != vector(1,0) )

		assertFalse( vector(1, 3) < vector(2,2) )
		assertFalse( vector(3, 1) < vector(2,2) )
		assertFalse( vector(1, 3) <= vector(2,2) )
		assertFalse( vector(3, 1) <= vector(2,2) )

		assertFalse( vector(3, 1) > vector(2,2) )
		assertFalse( vector(1, 3) > vector(2,2) )
		assertFalse( vector(3, 1) >= vector(2,2) )
		assertFalse( vector(1, 3) >= vector(2,2) )

		assertFalse( vector(1, 3) == vector(2,2) )
		assertFalse( vector(3, 1) == vector(2,2) )

		assertTrue( vector(1, 1) < vector(2,2) )
		assertTrue( vector(1, 1) <= vector(2,2) )
		assertTrue( vector(3, 3) > vector(2,2) )
		assertTrue( vector(3, 3) >= vector(2,2) )
		assertTrue( vector(2, 2) == vector(2,2) )
	)
)

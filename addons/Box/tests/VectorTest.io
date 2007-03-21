Vector

VectorTest := UnitTest clone do(
	type := "VectorTest"
	
	test_1_Add := method(		
		self v1 := vector(1, 2, 3)
		self v2 := vector(4, 5, 6)
		
		v3 := v1 + v2
		v4 := vector(5, 7, 9)
		assertTrue(v3 == v4)
	)
		
	test_2_Subtract := method(		
		v3 := v2 - v1
		v4 := vector(3, 3, 3)
		assertTrue(v3 == v4)
	)
		
	test_3_Multiply := method(		
		v3 := v2 * v1
		v4 := vector(4, 10, 18)
		assertTrue(v3 == v4)
	)
		
	test_4_Dot := method(		
		v3 := v1 dot(v2)
		v4 := 32
		assertTrue(v3 == v4)
	)
	
	test_5_Max := method(
		v3 := vector(3, 3, 3)
		v4 := vector(4, 4, 4)
		v3 Max(v4)
		assertTrue(v3 == v4)
	)

	test_6_Min := method(
		v3 := vector(3, 3, 3)
		v4 := vector(4, 4, 4)
		v4 Min(v3)
		assertTrue(v3 == v4)
	)

	testEquals := method(
		assertTrue(vector(1,2,3) == vector(1,2,3))
		assertFalse(vector(4,2,3) == vector(1,2,3))
	)

	testNotEquals := method(
		assertFalse(vector(1,2,3) != vector(1,2,3))
		assertTrue(vector(4,2,3) != vector(1,2,3))
	)
)

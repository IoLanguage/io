
LoopTest := UnitTest clone do(

	testLoop1 := method(
		loop(
			break
			fail
		)
	)

	testLoop2 := method(
		i := 0
		loop(
			i = i + 1
			if( i < 10, continue, break)
			fail
		)
		assertTrue( i == 10 )
	)

	testLoop3 := method(
		i := 0
		loop(
			i = i + 1
			if( i >= 10, break, continue)
			fail
		)
		assertTrue( i == 10 )
	)
)

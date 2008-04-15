SwitchTest := UnitTest clone do(

	testCommonCaseWithDefault := method(
		assertEquals(
			list(0, 1,2,3,4,5, 6,7) map(x,
				x switch(
					1, "one",
					2, "two",
					3, "three",
					4, "four",
					5, "five",
					"default"
				)
			)
		,
			list("default", "one", "two", "three", "four", "five", "default", "default")
		)
	)

	testCommonCaseNoDefault := method(
		assertEquals(
			list(0, 1,2,3,4,5, 6,7) map(x,
				x switch(
					1, "one",
					2, "two",
					3, "three",
					4, "four",
					5, "five"
				)
			)
		,
			list(nil, "one", "two", "three", "four", "five", nil, nil)
		)
	)

	testSingleArg := method(
		assertEquals(nil switch(13), 13)
		assertEquals(Object switch(13), 13)
		assertEquals(1 switch(13), 13)
		assertEquals("abc" switch(13), 13)
	)

	testNoArgs := method(
		assertNil(nil switch)
		assertNil(Object switch)
		assertNil(1 switch)
		assertNil("abc" switch)
	)

	testBreakAndContinueInsideSwitch := method(
		# break and continue operate on the loop
		r := list
		10 repeat(i,
			i switch(
				1, r append("one")
			,
				2, r append("two")
			,
				3,
				r append("three")
				continue
			,
				4, r append("four")
			,
				5, break
			,
				r append(i)
			)
			r append(i)
		)

		assertEquals(r, list(0, 0, "one", 1, "two", 2, "three", "four", 4))
	)

	testReturnInsideSwitch := method(
		r := list
		f := method(
			3 switch(
				1, r append(1),
				2, r append(2),
				3, r append(3); return 13; r append("3.1"),
				4, r append(4),
				r append(5)
			)
			r append(6)
		)

		assertEquals(f, 13)
		assertEquals(r, list(3))
	)

	if(isLaunchScript,
		verbose := getSlot("writeln")
		run
	)
)
